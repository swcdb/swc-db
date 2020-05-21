
/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 */ 

#include "swcdb/db/Types/MetaColumn.h"
#include "swcdb/db/client/Clients.h"
#include "swcdb/db/client/Query/Update.h"
#include "swcdb/db/Protocol/Rgr/req/RangeQueryUpdate.h"


namespace SWC { namespace client { namespace Query {

namespace Result {

uint32_t Update::completion() {
  Mutex::scope lock(m_mutex);
  return m_completion;
}

void Update::completion_incr() {
  Mutex::scope lock(m_mutex);
  ++m_completion;
}

void Update::completion_decr() {
  Mutex::scope lock(m_mutex);
  --m_completion;
}

int Update::error() {
  Mutex::scope lock(m_mutex);
  return m_err;
}

void Update::error(int err) {
  Mutex::scope lock(m_mutex);
  m_err = err;
}

void Update::add_resend_count(size_t count) {
  Mutex::scope lock(m_mutex);
  m_resend_cells += count;
}

size_t Update::get_resend_count(bool reset) {
  size_t sz;
  Mutex::scope lock(m_mutex);
  sz = m_resend_cells;
  if(reset)
    m_resend_cells = 0;
  return sz;
}

}



Update::Update(Cb_t cb)
        : buff_sz(Env::Clients::ref().cfg_send_buff_sz->get()), 
          buff_ahead(Env::Clients::ref().cfg_send_ahead->get()), 
          timeout(Env::Clients::ref().cfg_send_timeout->get()), 
          timeout_ratio(Env::Clients::ref().cfg_send_timeout_ratio->get()),
          cb(cb),
          columns(std::make_shared<DB::Cells::MapMutable>()),
          columns_onfractions(std::make_shared<DB::Cells::MapMutable>()),
          result(std::make_shared<Result>()) { 
}

Update::Update(DB::Cells::MapMutable::Ptr columns, 
         DB::Cells::MapMutable::Ptr columns_onfractions, 
         Cb_t cb)
        : buff_sz(Env::Clients::ref().cfg_send_buff_sz->get()), 
          buff_ahead(Env::Clients::ref().cfg_send_ahead->get()), 
          timeout(Env::Clients::ref().cfg_send_timeout->get()), 
          timeout_ratio(Env::Clients::ref().cfg_send_timeout_ratio->get()),
          cb(cb), 
          columns(columns), 
          columns_onfractions(columns_onfractions), 
          result(std::make_shared<Result>()) { 
}

Update::~Update() { }
 
void Update::response(int err) {
  if(result->completion() > 1) {
    result->completion_decr();
    return;
  }

  if(!err && (columns->size() || columns_onfractions->size())) {
    commit();
    result->completion_decr();
    return;
  }

  result->completion_decr();

  if(err)
    result->error(err);

  result->profile.finished();
  if(cb)
    cb(result);

  cv.notify_all();
}

void Update::wait() {
  std::unique_lock lock_wait(m_mutex);
  if(result->completion()) {
    cv.wait(
      lock_wait, 
      [updater=shared_from_this()]() {
        return !updater->result->completion();
      }
    );
  }
}


void Update::commit_or_wait() {
  size_t bytes = columns->size_bytes() 
                + columns_onfractions->size_bytes();

  if(result->completion() && bytes >= buff_sz * buff_ahead)
    wait();
  if(!result->completion() && bytes >= buff_sz)
    commit();
}

void Update::commit_or_wait(DB::Cells::ColCells::Ptr& col) {
  size_t bytes = col->size_bytes();

  if(result->completion() && bytes >= buff_sz * buff_ahead)
    wait();
  if(!result->completion() && bytes >= buff_sz)
    commit(col);
}

void Update::commit_if_need() {
  size_t bytes = columns->size_bytes() 
                + columns_onfractions->size_bytes();
  if(!result->completion() && bytes)
    commit();
}
  

void Update::commit() {
  DB::Cells::ColCells::Ptr col;
  for(size_t idx=0; (col=columns->get_idx(idx)) != nullptr; ++idx)
    commit(col);
  for(size_t idx=0; (col=columns_onfractions->get_idx(idx)) != nullptr; ++idx)
    commit_onfractions(col);
}

void Update::commit(const int64_t cid) { 
  commit(columns->get_col(cid));
  commit_onfractions(columns_onfractions->get_col(cid));
}

void Update::commit(DB::Cells::ColCells::Ptr col) {
  if(col != nullptr && !col->size())
    return;
  std::make_shared<Locator>(
    Types::Range::MASTER, 
    col->cid, col, col->get_first_key(), 
    shared_from_this()
  )->locate_on_manager();
}

void Update::commit_onfractions(DB::Cells::ColCells::Ptr col) {
  if(col != nullptr && !col->size())
    return;
  // query cells on fractions -EQ && rest(NONE-true)
  // update cell with + ts,flag,value and add cell to 'columns'
}



Update::Locator::Locator(const Types::Range type, const int64_t cid, 
        DB::Cells::ColCells::Ptr col, DB::Cell::Key::Ptr key_start,
        Update::Ptr updater, ReqBase::Ptr parent, 
        const int64_t rid, const DB::Cell::Key* key_finish) 
        : type(type), cid(cid), col(col), key_start(key_start), 
          updater(updater), parent(parent), 
          rid(rid), 
          key_finish(key_finish ? *key_finish : DB::Cell::Key()) {
}

Update::Locator::~Locator() { }

std::string Update::Locator::to_string() {
  std::string s("Locator(type=");
  s.append(Types::to_string(type));
  s.append(" cid=");
  s.append(std::to_string(cid));
  s.append(" rid=");
  s.append(std::to_string(rid));
  s.append(" completion=");
  s.append(std::to_string(updater->result->completion()));
  s.append(" Start");
  s.append(key_start->to_string());
  s.append(" Finish");
  s.append(key_finish.to_string());
  s.append(" ");
  s.append(col->to_string());
  return s;
}

void Update::Locator::locate_on_manager() {
  updater->result->completion_incr();

  Protocol::Mngr::Params::RgrGetReq params(
    Types::MetaColumn::get_master_cid(col->get_sequence()));
  params.range_begin.copy(*key_start.get());
  if(Types::MetaColumn::is_data(cid))
    params.range_begin.insert(0, std::to_string(cid));
  if(!Types::MetaColumn::is_master(cid))
    params.range_begin.insert(
      0, Types::MetaColumn::get_meta_cid(col->get_sequence()));

  SWC_LOGF(LOG_DEBUG, "LocateRange-onMngr %s", params.to_string().c_str());

  Protocol::Mngr::Req::RgrGet::request(
    params,
    [profile=updater->result->profile.mngr_locate(), 
     locator=shared_from_this()]
    (ReqBase::Ptr req, const Protocol::Mngr::Params::RgrGetRsp& rsp) {
      profile.add(rsp.err || !rsp.rid);
      if(locator->located_on_manager(req, rsp))
        locator->updater->result->completion_decr();
    }
  );
}


bool Update::Locator::located_on_manager(
      const ReqBase::Ptr& base, 
      const Protocol::Mngr::Params::RgrGetRsp& rsp) {
  SWC_LOGF(LOG_DEBUG, "LocatedRange-onMngr %s", rsp.to_string().c_str());
  
  if(rsp.err == Error::COLUMN_NOT_EXISTS) {
    updater->response(rsp.err);
    return false;
  }
  if(rsp.err || !rsp.rid) {
    SWC_LOGF(LOG_DEBUG, "Located-onMngr RETRYING %s", 
                          rsp.to_string().c_str());
    base->request_again();
    return false;
  }
  
  auto locator = std::make_shared<Locator>(
    Types::Range::MASTER, 
    rsp.cid, col, key_start, 
    updater, base, 
    rsp.rid
      );
  if(Types::MetaColumn::is_master(col->cid))
    locator->commit_data(rsp.endpoints, base);
  else 
    locator->locate_on_ranger(rsp.endpoints);

  if(!rsp.range_end.empty()) {
    auto next_key_start = col->get_key_next(rsp.range_end);
    if(next_key_start != nullptr) {
      std::make_shared<Locator>(
        Types::Range::MASTER, 
        col->cid, col, next_key_start, 
        updater
          )->locate_on_manager();
    }
  }
  return true;
}

void Update::Locator::locate_on_ranger(const EndPoints& endpoints) {
  updater->result->completion_incr();

  Protocol::Rgr::Params::RangeLocateReq params(cid, rid);
  params.flags |= Protocol::Rgr::Params::RangeLocateReq::COMMIT;

  params.range_begin.copy(*key_start.get());
  if(!Types::MetaColumn::is_master(col->cid)) {
    params.range_begin.insert(0, std::to_string(col->cid));
    if(type == Types::Range::MASTER && Types::MetaColumn::is_data(col->cid))
      params.range_begin.insert(
        0, Types::MetaColumn::get_meta_cid(col->get_sequence()));
  }
  
  SWC_LOGF(LOG_DEBUG, "LocateRange-onRgr %s", params.to_string().c_str());

  Protocol::Rgr::Req::RangeLocate::request(
    params, endpoints,
    [profile=updater->result->profile.rgr_locate(type),
     locator=shared_from_this()]
    (ReqBase::Ptr req, const Protocol::Rgr::Params::RangeLocateRsp& rsp) {
      profile.add(!rsp.rid || rsp.err);
      if(locator->located_on_ranger(
          std::dynamic_pointer_cast<Protocol::Rgr::Req::RangeLocate>(req)->endpoints,
          req, rsp))
        locator->updater->result->completion_decr();
    }
  );
}

bool Update::Locator::located_on_ranger(
      const EndPoints& endpoints, 
      const ReqBase::Ptr& base, 
      const Protocol::Rgr::Params::RangeLocateRsp& rsp) {
  SWC_LOGF(LOG_DEBUG, "LocatedRange-onRgr %s", rsp.to_string().c_str());

  if(rsp.err == Error::RS_NOT_LOADED_RANGE || 
     rsp.err == Error::RANGE_NOT_FOUND || //onMngr can be COLUMN_NOT_EXISTS
         rsp.err == Error::SERVER_SHUTTING_DOWN ||
     rsp.err == Error::COMM_NOT_CONNECTED) {
    SWC_LOGF(LOG_DEBUG, "Located-onRgr RETRYING %s", 
                          rsp.to_string().c_str());
    Env::Clients::get()->rangers.remove(cid, rid);
    parent->request_again();
    return false;
  }
  if(!rsp.rid || rsp.err) {
    SWC_LOGF(LOG_DEBUG, "Located-onRgr RETRYING %s", 
                          rsp.to_string().c_str());
    if(rsp.err != Error::REQUEST_TIMEOUT)
      Env::Clients::get()->rangers.remove(cid, rid);
    base->request_again();
    return false;
  }

  std::make_shared<Locator>(
    type == Types::Range::MASTER ? Types::Range::META : Types::Range::DATA,
    rsp.cid, col, key_start, 
    updater, base, 
    rsp.rid, &rsp.range_end
      )->resolve_on_manager();

  if(!rsp.range_end.empty()) {
    auto next_key_start = col->get_key_next(rsp.range_end);
    if(next_key_start != nullptr) {
      std::make_shared<Locator>(
        type, 
        cid, col, next_key_start, 
        updater, base, 
        rid//, &key_finish
          )->locate_on_ranger(endpoints);
    }
  }
  return true;
}

void Update::Locator::resolve_on_manager() {

  auto req = Protocol::Mngr::Req::RgrGet::make(
    Protocol::Mngr::Params::RgrGetReq(cid, rid),
    [profile=updater->result->profile.mngr_res(), locator=shared_from_this()]
    (ReqBase::Ptr req, const Protocol::Mngr::Params::RgrGetRsp& rsp) {
      profile.add(rsp.err || !rsp.rid || rsp.endpoints.empty());
      if(locator->located_ranger(req, rsp))
        locator->updater->result->completion_decr();
    }
  );
  if(!Types::MetaColumn::is_master(cid)) {
    Protocol::Mngr::Params::RgrGetRsp rsp(cid, rid);
    if(Env::Clients::get()->rangers.get(cid, rid, rsp.endpoints)) {
      SWC_LOGF(LOG_DEBUG, "Cache hit %s", rsp.to_string().c_str());
      if(proceed_on_ranger(req, rsp)) // ?req without profile
        return; 
      Env::Clients::get()->rangers.remove(cid, rid);
    } else {
      SWC_LOGF(LOG_DEBUG, "Cache miss %s", rsp.to_string().c_str());
    }
  }
  updater->result->completion_incr();
  req->run();
}

bool Update::Locator::located_ranger(
      const ReqBase::Ptr& base, 
      const Protocol::Mngr::Params::RgrGetRsp& rsp) {
  SWC_LOGF(LOG_DEBUG, "LocatedRanger-onMngr %s", rsp.to_string().c_str());

  if(rsp.err) {
    if(rsp.err == Error::COLUMN_NOT_EXISTS) {
      updater->response(rsp.err);
      return false;
    }
    SWC_LOGF(LOG_DEBUG, "LocatedRanger-onMngr RETRYING %s", 
                          rsp.to_string().c_str());
    SWC_LOGF(LOG_DEBUG, " %s", to_string().c_str());
    if(rsp.err == Error::RANGE_NOT_FOUND) {
      (parent == nullptr ? base : parent)->request_again();
    } else {
      base->request_again();
    }
    return false;
  }

  if(!rsp.rid || rsp.endpoints.empty()) {
    SWC_LOGF(LOG_DEBUG, "LocatedRanger-onMngr RETRYING(no rid) %s", 
                        rsp.to_string().c_str());
    (parent == nullptr ? base : parent)->request_again();
    return false;
  }

  if(!Types::MetaColumn::is_master(rsp.cid))
    Env::Clients::get()->rangers.set(rsp.cid, rsp.rid, rsp.endpoints);

  return proceed_on_ranger(base, rsp);
}

bool Update::Locator::proceed_on_ranger(
      const ReqBase::Ptr& base, 
      const Protocol::Mngr::Params::RgrGetRsp& rsp) {
  if(type == Types::Range::DATA || 
    (type == Types::Range::MASTER && Types::MetaColumn::is_master(col->cid)) ||
    (type == Types::Range::META   && Types::MetaColumn::is_meta(col->cid) )) {
    if(cid != rsp.cid || col->cid != cid) {
      SWC_LOGF(LOG_DEBUG, "LocatedRanger-onMngr RETRYING(cid no match) %s", 
                            rsp.to_string().c_str());
      (parent == nullptr ? base : parent)->request_again();
      //updater->response(Error::NOT_ALLOWED);
      return false;
    }
    commit_data(rsp.endpoints, base);

  } else {
    std::make_shared<Locator>(
      type, 
      rsp.cid, col, key_start, 
      updater, base, 
      rsp.rid, 
      Types::MetaColumn::is_master(cid) ? &rsp.range_end : nullptr
        )->locate_on_ranger(rsp.endpoints);
  }
  return true;
}

void Update::Locator::commit_data(
      EndPoints endpoints,
      const ReqBase::Ptr& base) {
  bool more = true;
  DynamicBuffer::Ptr cells_buff;
  auto workload = std::make_shared<bool>();
  while(more && 
       (cells_buff = col->get_buff(
         *key_start.get(), key_finish, updater->buff_sz, more)) 
                                                        != nullptr) {
    updater->result->completion_incr();

    Protocol::Rgr::Req::RangeQueryUpdate::request(
      Protocol::Rgr::Params::RangeQueryUpdateReq(col->cid, rid), 
      cells_buff, 
      endpoints, 
      [workload, cells_buff, base, 
       profile=updater->result->profile.rgr_data(), locator=shared_from_this()]
      (ReqBase::Ptr req, 
       const Protocol::Rgr::Params::RangeQueryUpdateRsp& rsp) {
        profile.add(rsp.err);

        if(rsp.err) {
          SWC_LOGF(LOG_DEBUG, "Commit RETRYING %s buffs=%d", 
                   rsp.to_string().c_str(), workload.use_count());

          if(rsp.err == Error::REQUEST_TIMEOUT) {
            SWC_LOGF(LOG_DEBUG, " %s", req->to_string().c_str());
            req->request_again();
            return;
          }

          if(rsp.err == Error::RANGE_BAD_INTERVAL) {
            size_t resend = locator->col->add(
              *cells_buff.get(), rsp.range_prev_end, rsp.range_end);
            locator->updater->result->add_resend_count(resend);

            if(workload.use_count() == 1) {
              auto nxt_start = locator->col->get_key_next(rsp.range_end);
              if(nxt_start != nullptr) {
                std::make_shared<Locator>(
                  Types::Range::MASTER,
                  locator->col->cid, locator->col, nxt_start,
                  locator->updater
                    )->locate_on_manager();
               }
            }
          } else {
            locator->updater->result->add_resend_count(
              locator->col->add(*cells_buff.get())
            );
            if(workload.use_count() == 1) {
              base->request_again();
              return;
            }
          }
        }

        locator->updater->response();
      },

      updater->timeout + cells_buff->fill()/updater->timeout_ratio
        );

  } while(more);
}



}}}
