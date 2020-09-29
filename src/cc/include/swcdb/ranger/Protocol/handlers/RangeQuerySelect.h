/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */


#ifndef swc_ranger_Protocol_handlers_RangeQuerySelect_h
#define swc_ranger_Protocol_handlers_RangeQuerySelect_h

#include "swcdb/db/Protocol/Rgr/params/RangeQuerySelect.h"
#include "swcdb/ranger/callbacks/RangeQuerySelect.h"


namespace SWC { namespace Protocol { namespace Rgr { namespace Handler {


void range_query_select(const Comm::ConnHandlerPtr& conn, const Comm::Event::Ptr& ev) {
  int err = Error::OK;
  Params::RangeQuerySelectReq params;
  Ranger::RangePtr range;

  try {
    const uint8_t *ptr = ev->data.base;
    size_t remain = ev->data.size;
    params.decode(&ptr, &remain);

    range = Env::Rgr::columns()->get_range(err, params.cid, params.rid);
 
    if(!err && (!range || !range->is_loaded()))
      err = Error::RGR_NOT_LOADED_RANGE;

  } catch(...) {
    const Exception& e = SWC_CURRENT_EXCEPTION("");
    SWC_LOG_OUT(LOG_ERROR, SWC_LOG_OSTREAM << e; );
    err = e.code();
  }

  try{
      
    if(err) {
      Protocol::Rgr::Params::RangeQuerySelectRsp rsp_params(err);
      auto cbp = Comm::CommBuf::make(rsp_params);
      cbp->header.initialize_from_request_header(ev->header);
      conn->send_response(cbp);
      return;
    }
    
    params.interval.apply_possible_range(
      params.interval.range_begin, params.interval.range_end);

    range->scan(
      std::make_shared<Ranger::Callback::RangeQuerySelect>(
        conn, ev, params.interval, range) );

  } catch(...) {
    SWC_LOG_CURRENT_EXCEPTION("");
  }
  
}
  

}}}}

#endif // swc_ranger_Protocol_handlers_RangeQuerySelect_h