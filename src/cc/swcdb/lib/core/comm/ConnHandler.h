/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */


#ifndef swc_core_comm_ConnHandler_h
#define swc_core_comm_ConnHandler_h

#include <asio.hpp>
#include "swcdb/lib/core/comm/Resolver.h"

namespace SWC { 

typedef std::shared_ptr<asio::io_context> IOCtxPtr;
typedef std::shared_ptr<asio::ip::tcp::socket> SocketPtr;
typedef std::shared_ptr<asio::high_resolution_timer> TimerPtr;
}

#include <memory>
#include "DispatchHandler.h"

#include "Event.h"
#include "CommBuf.h"
#include "Protocol.h"


namespace SWC { 


size_t endpoints_hash(EndPoints endpoints){
  std::string s;
  for(auto e : endpoints){
    s.append(e.address().to_string());
    s.append(":");
    s.append(std::to_string(e.port()));
  }
  std::hash<std::string> hasher;
  return hasher(s);
}

size_t endpoint_hash(asio::ip::tcp::endpoint endpoint){
  std::hash<std::string> hasher;
  return hasher(
    (std::string)endpoint.address().to_string()
    +":"
    +std::to_string(endpoint.port()));
}

class ConnHandler : public std::enable_shared_from_this<ConnHandler> {

  struct PendingRsp {
    uint32_t           id;
    DispatchHandlerPtr hdlr;
    TimerPtr           tm;
    EventPtr           ev;
  };

  public:
  ConnHandler(AppContextPtr app_ctx, SocketPtr socket, IOCtxPtr io_ctx) 
            : m_app_ctx(app_ctx), m_sock(socket), m_io_ctx(io_ctx)
            //, m_strand_out(*io_ctx.get()), m_strand_in(*io_ctx.get())
  {
    //new_connection();
    //m_app_ctx->stats->connected();
  }

  virtual ~ConnHandler(){ 
    try{m_sock->close();}catch(...){}
    //m_app_ctx->stats->disconnected();
  }
  
  EndPoint      endpoint_remote;
  EndPoint      endpoint_local;
  AppContextPtr m_app_ctx;
  SocketPtr     m_sock;
  IOCtxPtr      m_io_ctx;

  const std::string endpoint_local_str(){
    std::string s(endpoint_local.address().to_string());
    s.append(":");
    s.append(std::to_string(endpoint_local.port()));
    return s;
  }
  
  const std::string endpoint_remote_str(){
    std::string s(endpoint_remote.address().to_string());
    s.append(":");
    s.append(std::to_string(endpoint_remote.port()));
    return s;
  }
  
  const size_t endpoint_remote_hash(){
    return endpoint_hash(endpoint_remote);
  }
  
  const size_t endpoint_local_hash(){
    return endpoint_hash(endpoint_local);
  }
  
  virtual void new_connection(){
    endpoint_remote = m_sock->remote_endpoint();
    endpoint_local = m_sock->local_endpoint();
    HT_INFOF("new_connection local=%s, remote=%s, executor=%d",
              endpoint_local_str().c_str(), endpoint_remote_str().c_str(),
              (size_t)&m_sock->get_executor().context());
  }

  inline bool is_open() {
    return m_sock->is_open() && m_err == Error::OK;
  }

  virtual void run(EventPtr ev, DispatchHandlerPtr hdlr=nullptr) {
    HT_WARNF("run is Virtual!, %s", ev->to_str().c_str());
    if(hdlr != nullptr)
      hdlr->handle(this, ev);
  }

  void do_close(DispatchHandlerPtr hdlr=nullptr){
    if(m_err == Error::OK) {
      m_err = Error::COMM_NOT_CONNECTED;
      m_pendings = 0;
    
      try{m_sock->close();}catch(...){}
      disconnected();
      run(std::make_shared<Event>(Event::Type::DISCONNECT, m_err), hdlr);
    }
  }

  int send_error(int error, const String &msg, EventPtr ev=nullptr) {
    if(m_err != Error::OK)
      return m_err;

    CommHeader header;
    if(ev != nullptr)
      header.initialize_from_request_header(ev->header);
    
    CommBufPtr cbp;
    size_t max_msg_size = std::numeric_limits<int16_t>::max();
    if (msg.length() < max_msg_size)
      cbp = Protocol::create_error_message(header, error, msg.c_str());
    else {
      cbp = Protocol::create_error_message(header, error, msg.substr(0, max_msg_size).c_str());
    }
    return send_response(cbp);
  }

  int response_ok(EventPtr ev=nullptr) {
    if(m_err != Error::OK)
      return m_err;
      
    CommHeader header;
    if(ev != nullptr)
      header.initialize_from_request_header(ev->header);
    CommBufPtr cbp = std::make_unique<CommBuf>(header, 4);
    cbp->append_i32(Error::OK);
    return send_response(cbp);
  }

  inline int send_response(CommBufPtr &cbuf, DispatchHandlerPtr hdlr=nullptr){
    if(m_err != Error::OK)
      return m_err;

    cbuf->header.flags &= CommHeader::FLAGS_MASK_REQUEST;
    cbuf->write_header_and_reset();
    
    write(cbuf, 
      get_timer(cbuf->header.timeout_ms, hdlr, cbuf->header.id, cbuf->header), 
      hdlr
    ); 
    return m_err;
  }

  inline int send_response(CommBufPtr &cbuf, uint32_t timeout_ms){
    if(m_err != Error::OK)
      return m_err;

    int e = Error::OK;
    cbuf->header.flags &= CommHeader::FLAGS_MASK_REQUEST;
    cbuf->write_header_and_reset();

    std::future<size_t> f = asio::async_write(
        *m_sock.get(), cbuf->get_buffers(), asio::use_future);
    std::future_status status = f.wait_for(
      std::chrono::milliseconds(timeout_ms));

    if (status == std::future_status::ready){
      try {
        f.get();
      } catch (const std::exception& ex) {
        e = (m_err != Error::OK? m_err.load() : Error::COMM_BROKEN_CONNECTION);
      }
    } else {
      e = Error::REQUEST_TIMEOUT;
    }
    read_pending();
    return e;
  }

  /* 
  inline int send_response_sync(CommBufPtr &cbuf){
    if(m_err != Error::OK)
      return m_err;

    asio::error_code error;
    uint32_t len_sent = m_sock->write_some(cbuf->get_buffers(), error);
    return error.value() ?
           (m_err != Error::OK? m_err.load() : Error::COMM_BROKEN_CONNECTION) 
           : Error::OK;
  }
  */

  inline int send_request(uint32_t timeout_ms, CommBufPtr &cbuf, 
                    DispatchHandlerPtr hdlr) {
    cbuf->header.timeout_ms = timeout_ms;
    return send_request(cbuf, hdlr);
  }

  inline int send_request(CommBufPtr &cbuf, DispatchHandlerPtr hdlr) {
    if(m_err != Error::OK)
      return m_err;

    cbuf->header.flags |= CommHeader::FLAGS_BIT_REQUEST;

    if(cbuf->header.id == 0)
      cbuf->header.id = next_req_id();

    cbuf->write_header_and_reset();
    
    write(cbuf, 
      get_timer(cbuf->header.timeout_ms, hdlr, cbuf->header.id, cbuf->header), 
      hdlr
    );
    return m_err;
  }

  inline void accept_requests() {
    m_pendings++;
    read_pending();
  }

  inline void accept_requests(DispatchHandlerPtr hdlr, 
                              uint32_t timeout_ms=0) {
    read_pending({
      .id=0,  // initial req.acceptor
      .hdlr=hdlr,
      .tm=get_timer(timeout_ms, hdlr, 0)
    });
  }

  private:

  inline uint32_t next_req_id(){
    uint32_t id = ++m_next_req_id;
    if(id == 0)// if uint32_t turns-back
      id = ++m_next_req_id;
    return id;
  }

  TimerPtr get_timer(uint32_t t_ms,  DispatchHandlerPtr hdlr, 
                     uint32_t id, CommHeader hr=0){
    if(t_ms == 0) return nullptr;
    TimerPtr tm = std::make_shared<asio::high_resolution_timer>(
      m_sock->get_executor(), std::chrono::milliseconds(t_ms)); 

    tm->async_wait([this, id, hr](const asio::error_code ec) {
      if (ec != asio::error::operation_aborted){
        EventPtr ev = std::make_shared<Event>(
          Event::Type::ERROR, Error::Code::REQUEST_TIMEOUT);
        ev->header = hr;
        run_pending(id, ev, true);
      }
    });  
    return tm;
  }
    
  void write(CommBufPtr &cbuf, TimerPtr tm, DispatchHandlerPtr hdlr){

    asio::async_write(*m_sock.get(), cbuf->get_buffers(),
      //asio::bind_executor(m_strand_out,
      [this, tm, hdlr, header=cbuf->header](
        const asio::error_code ec, uint32_t len) {
        if(ec)
         do_close();
        else if(header.flags & CommHeader::FLAGS_BIT_REQUEST)
          read_pending({.id=header.id, .hdlr=hdlr, .tm=tm});
        else {    
          m_pendings++;
          read_pending();
        }
        
      }
      //)
    );
  }

  void read_pending(PendingRsp q){        
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_pending.push_back(q);
      /*
        std::cout << "read_pending: pending(sz=" << m_pending.size() 
                                << " cap=" << m_pending.capacity() 
                                << "), cancelled(sz=" << m_cancelled.size() 
                                << " cap=" << m_cancelled.capacity() << ") \n";
      */
    }
    m_pendings++;
    read_pending();
  }

  void read_pending(){
    if(m_reading || m_err != Error::OK)
      return;
    m_reading=true;

    uint8_t* data = new uint8_t[CommHeader::FIXED_LENGTH+1];
    EventPtr ev = std::make_shared<Event>(Event::Type::MESSAGE, Error::OK);

    asio::async_read(
      *m_sock.get(), 
      asio::mutable_buffer(data, CommHeader::FIXED_LENGTH+1),
      [this, ev, data](const asio::error_code ec, size_t filled) {
        asio::error_code e = ec;
        size_t remain = read_condition(ev, data, e, filled);
        //std::cout << "condition, remain=" << remain << ", e=" << e << "\n";
        if(e) {
          remain = 0;
          do_close();
        }
        if(remain == 0) 
          delete [] data;
        return remain;
      },
      //asio::bind_executor(m_strand_in,
      [this, ev](const asio::error_code e, uint32_t filled) { 
        ev->arrival_time = ClockT::now();
        //std::cout << "handler, m_pendings=" << m_pendings << ", e=" << e << "\n";
        e ? do_close() : ({
          m_reading = false;
          if(m_pendings > 0) {
            m_pendings--;
            read_pending();
          }
          run_pending(ev->header.id, ev);
        });
      }
      //)
    );
  }

  size_t read_condition(EventPtr ev, uint8_t* data,
                        asio::error_code &e, size_t filled){
                  
    if(filled < CommHeader::FIXED_LENGTH)
      return CommHeader::FIXED_LENGTH-filled;
      
    try{
      ev->load_message_header(data, filled);
    } catch(...){
      ev->type = Event::Type::ERROR;
      ev->error = Error::REQUEST_TRUNCATED;
      ev->header = CommHeader();
      
      HT_WARNF("read, REQUEST HEADER_TRUNCATED: filled=%d", filled);
      e = asio::error::eof;
      return (size_t)0;
    }

    if(ev->header.total_len == ev->header.header_len)
      return (size_t)0;

    ev->payload_len = ev->header.total_len-ev->header.header_len;
    uint8_t* payload = new uint8_t[ev->payload_len]; 
    ev->payload = payload; 
    
    asio::error_code ec;
    filled = 0;
    do{
      filled += m_sock->read_some(
        asio::mutable_buffer(payload+filled, ev->payload_len-filled),
        ec);
    } while (!ec && filled < ev->payload_len);

    if(filled != ev->payload_len) {
      ev->payload_len=0;
      e = ec;
    }
    if(e){ 
      HT_WARNF("read, REQUEST PAYLOAD_TRUNCATED: filled=%d error=%s ", 
              filled, ec, ev->to_str().c_str());
    }
    return (size_t)0;
  }

  void disconnected(){
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      /*
      std::cout << "disconnected: m_cancelled=" << m_cancelled.size() 
                << " m_pending=" << m_pending.size() << "\n";
      */
      m_cancelled.clear();
    }
    
    for(;;) {
      PendingRsp q;
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_pending.empty())
          return;
        q = *m_pending.begin();
        m_pending.erase(m_pending.begin());
      }
      
      if(q.tm != nullptr) q.tm->cancel();
      run(std::make_shared<Event>(Event::Type::DISCONNECT, m_err), q.hdlr);
    }
  }

  void run_pending(uint32_t id, EventPtr ev, bool cancelling=false){
    bool found_current = false;
    bool found_not = false;
    bool found_next;
    bool skip;
    std::vector<PendingRsp>::iterator it_found;
    std::vector<PendingRsp>::iterator it_end;

    PendingRsp q_now;
    PendingRsp q_next;
    do {
      found_next = false;
      skip = false;
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        /* 
        std::cout << "run_pending: m_cancelled=" << m_cancelled.size() 
                  << " m_pending=" << m_pending.size() << "\n";
        for(auto q :m_pending )
          std::cout << " id=" << q.id << "\n";
        std::cout << "looking for id=" << id << " fd="<<m_sock->native_handle()<<" ptr="<<this
        << " " << endpoint_local_str() <<"\n";
        */
        if(cancelling)
          m_cancelled.push_back(id);

        found_not = m_pending.empty();
        if(!found_not){
          it_found = std::find_if(m_pending.begin(), m_pending.end(), 
                                 [id](const PendingRsp & q){return q.id == id;});
          it_end = m_pending.end();
          found_not = it_found == it_end;
        }
        if(!found_not){
          found_current = it_found == m_pending.begin();

          if(found_current){
            q_now = *it_found;
            if(q_now.tm != nullptr) 
              q_now.tm->cancel();
            if(++it_found != it_end){
              found_next = (*it_found).ev != nullptr;
              if(found_next)
                q_next = *it_found;
            }
            m_pending.erase(--it_found);

          } else {
            (*it_found).ev=ev;
          }
        } else if(!cancelling) {
            auto it = std::find_if(m_cancelled.begin(), m_cancelled.end(), 
                                  [id](uint32_t i){return i == id;});
            skip = (it != m_cancelled.end());
            if(skip)
              m_cancelled.erase(it);
        }
      }
    
      if(found_not){
        if(id == 0)
          run(ev);
        else if(!skip)
          run_pending(0, ev);
        
      } else if(found_current){

        run(ev, q_now.hdlr);
        
        if(found_next)
          run_pending(q_next.id, q_next.ev);
      }
    } while(found_next);

  }

  std::vector<PendingRsp>   m_pending;
  std::vector<uint32_t>     m_cancelled;
  std::mutex                m_mutex;
  std::atomic<uint32_t>     m_pendings = 0;
  std::atomic<bool>         m_reading = 0;
  std::atomic<Error::Code>  m_err = Error::OK;
  
  std::atomic<uint32_t>     m_next_req_id;
  //
  //asio::io_context::strand  m_strand_in;
  //asio::io_context::strand  m_strand_out;

};


}

#endif // swc_core_comm_ConnHandler_h