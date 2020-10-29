/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */


#include "swcdb/db/Protocol/Rgr/params/RangeLoad.h"

namespace SWC { namespace Ranger { namespace Callback {


RangeLoad::RangeLoad(const Comm::ConnHandlerPtr& conn, 
                     const Comm::Event::Ptr& ev,
                     const cid_t cid, const rid_t rid)
                    : ManageBase(conn, ev, ManageBase::RANGE_LOAD), 
                      cid(cid), rid(rid) {
  Env::Rgr::in_process(1);
}

RangeLoad::~RangeLoad() { 
  Env::Rgr::in_process(-1);
}

void RangeLoad::loaded(int& err) {
  if(!err && (Env::Rgr::is_shuttingdown() ||
              (Env::Rgr::is_not_accepting() &&
               DB::Types::MetaColumn::is_data(cid))))
    err = Error::SERVER_SHUTTING_DOWN;

  RangePtr range;
  if(!err) {
    range = Env::Rgr::columns()->get_range(err, cid, rid);
    if(err || !range || !range->is_loaded())
      err = Error::RGR_NOT_LOADED_RANGE;
  }

  try {
    if(err) {
      col->internal_unload(rid);
      Env::Rgr::columns()->erase_if_empty(cid);

      if(!expired())
        m_conn->send_error(err, "", m_ev);

    } else if(!expired()) {
      Comm::Protocol::Rgr::Params::RangeLoaded params(range->cfg->key_seq);
      if((params.intval = range->cfg->range_type == DB::Types::Range::MASTER))
        range->get_interval(params.interval);
        
      auto cbp = Comm::Buffers::make(params, 4);
      cbp->header.initialize_from_request_header(m_ev->header);
      cbp->append_i32(err);
      m_conn->send_response(cbp);
    }
  } catch(...) {
    SWC_LOG_CURRENT_EXCEPTION("");
  }
  
  col->run_mng_queue();
}


}}}