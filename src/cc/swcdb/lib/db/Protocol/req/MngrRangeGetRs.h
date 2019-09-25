
/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */

#ifndef swc_lib_db_protocol_req_MngrRangeGetRs_h
#define swc_lib_db_protocol_req_MngrRangeGetRs_h


#include "swcdb/lib/db/Protocol/Commands.h"

#include "MngrMngrActive.h"
#include "../params/MngrRangeGetRs.h"


namespace SWC {
namespace Protocol {
namespace Req {

  
class MngrRangeGetRs: public ConnQueue::ReqBase {
  public:
  
  typedef std::function<
          void(ConnQueue::ReqBase::Ptr, Params::MngrRangeGetRsRsp)> Cb_t;

  static void request(int64_t cid, int64_t rid, 
                      const Cb_t cb, const uint32_t timeout = 10000){
    request(Protocol::Params::MngrRangeGetRsReq(cid, rid), cb, timeout);
  }

  static void request(int64_t cid, const DB::Specs::Interval::Ptr interval, 
                      const Cb_t cb, const uint32_t timeout = 10000){
    request(Protocol::Params::MngrRangeGetRsReq(cid, interval), cb, timeout);
  }
  static void request(int64_t cid, const DB::Cell::Key& key, 
                      const Cb_t cb, const uint32_t timeout = 10000){
    request(Protocol::Params::MngrRangeGetRsReq(cid, key), cb, timeout);
  }

  static inline void request(const Protocol::Params::MngrRangeGetRsReq params,
                             const Cb_t cb, const uint32_t timeout = 10000){
    std::make_shared<MngrRangeGetRs>(params, cb, timeout)->run();
  }


  MngrRangeGetRs(const Protocol::Params::MngrRangeGetRsReq params, const Cb_t cb, 
                 const uint32_t timeout) 
                : ConnQueue::ReqBase(false), cb(cb), cid(params.cid) {
    CommHeader header(Protocol::Command::CLIENT_REQ_GET_RANGE_RS, timeout);
    cbp = std::make_shared<CommBuf>(header, params.encoded_length());
    params.encode(cbp->get_data_ptr_address());
  }

  virtual ~MngrRangeGetRs(){}

  void handle_no_conn() override {
    clear_endpoints();
    run();
  }

  bool run(uint32_t timeout=0) override {
    if(endpoints.empty()){
      Env::Clients::get()->mngrs_groups->select(cid, endpoints); 
      if(endpoints.empty()){
        std::make_shared<MngrMngrActive>(cid, shared_from_this())->run();
        return false;
      }
    } 
    Env::Clients::get()->mngr->get(endpoints)->put(req());
    return true;
  }

  void handle(ConnHandlerPtr conn, EventPtr &ev) override {

    if(ev->type == Event::Type::DISCONNECT){
      handle_no_conn();
      return;
    }

    Protocol::Params::MngrRangeGetRsRsp rsp_params;
    try{
      const uint8_t *ptr = ev->payload;
      size_t remain = ev->payload_len;
      rsp_params.decode(&ptr, &remain);
    } catch (Exception &e) {
      HT_ERROR_OUT << e << HT_END;
      rsp_params.err = e.code();
    }
    cb(req(), rsp_params);
  }

  private:
  
  void clear_endpoints() {
    Env::Clients::get()->mngrs_groups->remove(endpoints);
    endpoints.clear();
  }

  const Cb_t  cb;
  EndPoints   endpoints;
  int64_t     cid;
};


}}}

#endif // swc_lib_db_protocol_req_MngrRangeGetRs_h
