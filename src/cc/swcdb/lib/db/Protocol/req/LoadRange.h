
/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */ 

#ifndef swc_lib_db_protocol_req_LoadRange_h
#define swc_lib_db_protocol_req_LoadRange_h

#include "swcdb/lib/db/Protocol/params/LoadRange.h"

namespace SWC {
namespace Protocol {
namespace Req {

class LoadRange : public DispatchHandler {
  public:

  LoadRange(client::ClientConPtr conn, 
            Types::RsRole role, uint64_t cid=0, uint64_t rid=0)
            : conn(conn), role(role), cid(cid), rid(rid) { }
  
  virtual ~LoadRange() { }
  
  bool run(uint32_t timeout=60000) override {
    Protocol::Params::LoadRange params = 
      Protocol::Params::LoadRange(role, cid, rid);
    
    CommHeader header(Protocol::Command::MNGR_REQ_LOAD_RANGE, timeout);
    CommBufPtr cbp = std::make_shared<CommBuf>(header, params.encoded_length());
    params.encode(cbp->get_data_ptr_address());

    return conn->send_request(cbp, shared_from_this()) == Error::OK;
  }

  void disconnected() {};

  void handle(ConnHandlerPtr conn, EventPtr &ev) {
    
    // HT_INFOF("%s", ev->to_str().c_str());
    
    if(ev->type == Event::Type::DISCONNECT){
      disconnected();
      return;
    }

    if(ev->header.command == Protocol::Command::MNGR_REQ_LOAD_RANGE 
       && Protocol::response_code(ev) == Error::OK){
      return;
    }

    conn->do_close();

  }

  private:
  Types::RsRole role;
  uint64_t  cid;
  uint64_t  rid;
  
  client::ClientConPtr conn;
  ;
};

typedef std::shared_ptr<LoadRange> LoadRangePtr;

}}}

#endif // swc_lib_db_protocol_req_LoadRange_h
