
/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */ 

#ifndef swc_manager_Protocol_rgr_req_RangeIsLoaded_h
#define swc_manager_Protocol_rgr_req_RangeIsLoaded_h

#include "swcdb/db/Protocol/Rgr/params/RangeIsLoaded.h"


namespace SWC { namespace Protocol { namespace Rgr { namespace Req {


class RangeIsLoaded : public client::ConnQueue::ReqBase {
  public:
  
  const Manager::ColumnHealthCheck::RangerCheck::Ptr   checker;
  const Manager::Range::Ptr                            range;

  RangeIsLoaded(const Manager::ColumnHealthCheck::RangerCheck::Ptr& checker,
                const Manager::Range::Ptr& range, uint32_t timeout=60000)
                : client::ConnQueue::ReqBase(false), 
                  checker(checker), range(range) { 
    cbp = CommBuf::make(Params::RangeIsLoaded(range->cfg->cid, range->rid));
    cbp->header.set(RANGE_IS_LOADED, timeout);
  }
  
  virtual ~RangeIsLoaded() { }
  
  bool valid() override {
    return checker->rgr->state == Types::MngrRanger::State::ACK && 
           range->assigned();
  }

  void handle_no_conn() {
    if(was_called)
      return;
    was_called = true;
    checker->handle(range, Error::COMM_CONNECT_ERROR);
  }

  void handle(ConnHandlerPtr, const Event::Ptr& ev) override {
    if(was_called)
      return;
    was_called = true;

    checker->handle(
      range, 
      ev->type == Event::Type::DISCONNECT
        ? Error::COMM_NOT_CONNECTED
        : ev->response_code());
  }

};

}}}}

#endif // swc_manager_Protocol_rgr_req_RangeIsLoaded_h
