
/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */ 

#ifndef swc_lib_db_protocol_common_req_QueryUpdate_h
#define swc_lib_db_protocol_common_req_QueryUpdate_h

#include "swcdb/db/Cells/SpecsScan.h"
#include "swcdb/db/Cells/MapMutable.h" 

#include "swcdb/db/Protocol/Mngr/req/RgrGet.h"
#include "swcdb/db/Protocol/Rgr/req/RangeLocate.h"
#include "swcdb/db/Protocol/Rgr/req/RangeQueryUpdate.h"
#include "swcdb/db/Types/Range.h"


namespace SWC { namespace Protocol { namespace Common { namespace Req { 
  
namespace Query {

using ReqBase = Req::ConnQueue::ReqBase;

/*
range-master: 
  req-mngr.   cid(1) + [n(cid), next_key_start]
              => cid(1) + rid + rgr(endpoints) + range_begin + range_end	
    req-rgr.  cid(1) + rid + [cid(n), next_key_start]
              => cid(2) + rid + range_begin + range_end
range-meta: 
  req-mngr.   cid(2) + rid                           
              => cid(2) + rid + rgr(endpoints)	
    req-rgr.  cid(2) + rid + [cid(n), next_key_start]
              => cid(n) + rid + range_begin + range_end
range-data: 
  req-mngr.   cid(n) + rid                           
              => cid(n) + rid + rgr(endpoints)	
    req-rgr.  cid(n) + rid + Specs::Interval         
              => results
*/
 
namespace Result{

struct Update final {
  typedef std::shared_ptr<Update> Ptr;
  int err;
  std::atomic<uint32_t> completion = 0;
  
  DB::Cells::MapMutable errored;
};

}
  

class Update : public std::enable_shared_from_this<Update> {
  public:

  using Result = Result::Update;

  typedef std::shared_ptr<Update>           Ptr;
  typedef std::function<void(Result::Ptr)>  Cb_t;
  
  Cb_t                        cb;
  DB::Cells::MapMutable::Ptr  columns;
  DB::Cells::MapMutable::Ptr  columns_onfractions;

  Result::Ptr                 result;

  uint32_t buff_sz          = 8000000;
  uint32_t timeout_commit   = 60000;
  uint32_t timeout_commit_bytes_ratio = 750;
  

  std::mutex                  m_mutex;
  std::condition_variable     cv;

  Update(Cb_t cb=0)
        : cb(cb),
          columns(std::make_shared<DB::Cells::MapMutable>()),
          columns_onfractions(std::make_shared<DB::Cells::MapMutable>()),
          result(std::make_shared<Result>()) { 
  }

  Update(DB::Cells::MapMutable::Ptr columns, 
         DB::Cells::MapMutable::Ptr columns_onfractions, 
         Cb_t cb=0)
        : cb(cb), 
          columns(columns), 
          columns_onfractions(columns_onfractions), 
          result(std::make_shared<Result>()) { 
  }

  virtual ~Update() { }
 
  void response(int err=Error::OK) {
    if(columns->size() || columns_onfractions->size()) {
      commit();
      return;
    }

    if(err)
      result->err=err;

    if(cb)
      cb(result);
    cv.notify_all();
  }

  void wait() {
    std::unique_lock<std::mutex> lock_wait(m_mutex);
    if(result->completion == 0)
      return;
    cv.wait(
      lock_wait, 
      [updater=shared_from_this()]() {
        return updater->result->completion==0;
      }
    );  
  }

  void commit() {
    DB::Cells::ColCells::Ptr col;
    for(size_t idx=0; (col=columns->get_idx(idx)) != nullptr; idx++)
      commit(col);
    for(size_t idx=0; (col=columns_onfractions->get_idx(idx)) != nullptr; idx++)
      commit_onfractions(col);
  }

  void commit(const int64_t cid) { 
    commit(columns->get_col(cid));
    commit_onfractions(columns_onfractions->get_col(cid));
  }

  void commit(DB::Cells::ColCells::Ptr col) {
    if(col != nullptr && !col->size())
      return;
    std::make_shared<Locator>(
      Types::Range::MASTER,
      col->cid, col, 
      col->get_first_key(), 
      shared_from_this()
    )->locate_on_manager();
  }

  void commit_onfractions(DB::Cells::ColCells::Ptr col) {
    if(col != nullptr && !col->size())
      return;
    // query cells on fractions -EQ && rest(NONE-true)
    // update cell with + ts,flag,value and add cell to 'columns'
  }

  class Locator : public std::enable_shared_from_this<Locator> {
    public:
    const Types::Range        type;
    const int64_t             cid;
    DB::Cells::ColCells::Ptr  col_cells;
    DB::Cell::Key::Ptr        key_start;
    Update::Ptr               updater;
    ReqBase::Ptr              parent;
    const int64_t             rid;
    
    Locator(const Types::Range type, const int64_t cid, 
            DB::Cells::ColCells::Ptr col_cells,
            DB::Cell::Key::Ptr key_start,
            Update::Ptr updater,
            ReqBase::Ptr parent=nullptr, const int64_t rid=0) 
            : type(type), cid(cid), col_cells(col_cells), 
              key_start(key_start), 
              updater(updater), parent(parent), rid(rid) {
    }

    virtual ~Locator() { }

    const std::string to_string() {
      std::string s("Locator(type=");
      s.append(Types::to_string(type));
      s.append(" cid=");
      s.append(std::to_string(cid));
      s.append(" rid=");
      s.append(std::to_string(rid));
      s.append(" completion=");
      s.append(std::to_string(updater->result->completion.load()));
      s.append(" ");
      s.append(col_cells->to_string());
      s.append(" ");
      s.append(key_start->to_string());
      return s;
    }

    void locate_on_manager() {
      updater->result->completion++;

      Mngr::Params::RgrGetReq params(1);
      
      params.range_begin.copy(*key_start.get());
      if(cid > 2)
        params.range_begin.insert(0, std::to_string(cid));
      if(cid >= 2)
        params.range_begin.insert(0, "2");

      Mngr::Req::RgrGet::request(
        params,
        [locator=shared_from_this()]
        (ReqBase::Ptr req, Mngr::Params::RgrGetRsp rsp) {
          if(locator->located_on_manager(req, rsp))
            locator->updater->result->completion--;
        }
      );
    }

    void resolve_on_manager() {
      updater->result->completion++;
      //std::cout << "resolve_on_manager:\n " << to_string() << "\n";
      // if cid, rid >> cache rsp

      Mngr::Req::RgrGet::request(
        Mngr::Params::RgrGetReq(cid, rid),
        [locator=shared_from_this()]
        (ReqBase::Ptr req, Mngr::Params::RgrGetRsp rsp) {
          if(locator->located_on_manager(req, rsp))
            locator->updater->result->completion--;
        }
      );
    }

    bool located_on_manager(const ReqBase::Ptr& base, 
                            const Mngr::Params::RgrGetRsp& rsp) {
      if(rsp.err) {
        if(rsp.err == Error::COLUMN_NOT_EXISTS) {
          updater->response(rsp.err);
          return true;
        }
        SWC_LOGF(LOG_DEBUG, "Located-onMngr RETRYING %s", 
                              rsp.to_string().c_str());
        if(rsp.err == Error::RANGE_NOT_FOUND) {
          (parent == nullptr ? base : parent)->request_again();
        } else {
          base->request_again();
        }
        return false;
      }
      if(!rsp.rid) {
        SWC_LOGF(LOG_DEBUG, "Located-onMngr RETRYING(no rid) %s", 
                            rsp.to_string().c_str());
        (parent == nullptr ? base : parent)->request_again();
        return false;
      }

      if(type == Types::Range::DATA || 
        (type == Types::Range::MASTER && col_cells->cid == 1) ||
        (type == Types::Range::META   && col_cells->cid == 2 )) {
        if(cid != rsp.cid || col_cells->cid != cid) {
          SWC_LOGF(LOG_DEBUG, "Located-onMngr RETRYING(cid no match) %s", 
                                rsp.to_string().c_str());
          (parent == nullptr ? base : parent)->request_again();
          return false;
          //updater->response(Error::NOT_ALLOWED);
        }

        commit_data(rsp.endpoints, rsp.rid, rsp.range_end, base);
        if(type == Types::Range::DATA)
          return true;
      
      } else {
        std::make_shared<Locator>(
          type, rsp.cid, col_cells, key_start, updater, 
          base, rsp.rid
        )->locate_on_ranger(rsp.endpoints);
      }

      if(!rsp.range_end.empty()) {
        auto next_key_start = col_cells->get_key_next(rsp.range_end);
        if(next_key_start != nullptr) {
          std::make_shared<Locator>(
            type, cid, col_cells, next_key_start, updater, 
            parent == nullptr ? base : parent
          )->locate_on_manager();
        }
      }
      return true;
    }

    void locate_on_ranger(const EndPoints& endpoints) {
      updater->result->completion++;

      Rgr::Params::RangeLocateReq params(cid, rid);

      params.range_begin.copy(*key_start.get());
      params.range_begin.insert(0, std::to_string(col_cells->cid));
      if(type == Types::Range::MASTER && col_cells->cid > 2) 
        params.range_begin.insert(0, "2");

      Rgr::Req::RangeLocate::request(
        params, 
        endpoints, 
        [locator=shared_from_this()]() {
          locator->parent->request_again();
        },
        [endpoints, locator=shared_from_this()] 
        (ReqBase::Ptr req, Rgr::Params::RangeLocateRsp rsp) {
          if(locator->located_on_ranger(endpoints, req, rsp))
            locator->updater->result->completion--;
        }
      );
    }

    bool located_on_ranger(const EndPoints& endpoints, 
                           const ReqBase::Ptr& base, 
                           const Rgr::Params::RangeLocateRsp& rsp) {
      if(rsp.err == Error::RS_NOT_LOADED_RANGE || 
         rsp.err == Error::RANGE_NOT_FOUND) {
        SWC_LOGF(LOG_DEBUG, "Located-onRgr RETRYING %s", 
                              rsp.to_string().c_str());
        parent->request_again();
        return false;
      }
      if(!rsp.rid || 
         (type == Types::Range::DATA && rsp.cid != col_cells->cid)) {
        SWC_LOGF(LOG_DEBUG, "Located-onRgr RETRYING(cid no match) %s", 
                              rsp.to_string().c_str());
        parent->request_again();
        return false;
      }
      if(rsp.err) {
        SWC_LOGF(LOG_DEBUG, "Located-onRgr HALT %s", 
                              rsp.to_string().c_str());
        quick_exit(1);
        base->request_again();
        return false;
      }

      /*
      auto range_next = get_key_next(rsp.range_end, true);
      if(range_next == nullptr) 
        return true;
      */

      std::make_shared<Locator>(
        type == Types::Range::MASTER ? Types::Range::META : Types::Range::DATA,
        rsp.cid, col_cells, key_start, updater, parent, rsp.rid
      )->resolve_on_manager();

      if(!rsp.range_end.empty()) {
        auto next_key_start = col_cells->get_key_next(rsp.range_end);
        if(next_key_start != nullptr) {
          std::make_shared<Locator>(
            type, cid, col_cells, next_key_start, updater, 
            parent == nullptr ? base : parent
          )->locate_on_ranger(endpoints);
        }
      }
      return true;
    }

    void commit_data(EndPoints endpoints, uint64_t rid,
                     const DB::Cell::Key& key_end,
                     const ReqBase::Ptr& base) {
      bool more = true;
      DynamicBuffer::Ptr cells_buff;
      while(more && 
           (cells_buff = col_cells->get_buff(
             *key_start.get(), key_end, updater->buff_sz, more)) != nullptr) {
        updater->result->completion++;
        
        Rgr::Req::RangeQueryUpdate::request(
          Rgr::Params::RangeQueryUpdateReq(col_cells->cid, rid), 
          cells_buff, 
          endpoints, 

          [cells_buff, base, locator=shared_from_this()]() {
            locator->col_cells->add(*cells_buff.get());
            SWC_LOG(LOG_DEBUG, "Commit RETRYING no-conn");
            base->request_again();
            --locator->updater->result->completion;
          },

          [cells_buff, base, locator=shared_from_this()] 
          (ReqBase::Ptr req, Rgr::Params::RangeQueryUpdateRsp rsp) {
            if(rsp.err) {
              SWC_LOGF(LOG_DEBUG, "Commit RETRYING %s", rsp.to_string().c_str());

              if(rsp.err == Error::REQUEST_TIMEOUT) {
                SWC_LOGF(LOG_DEBUG, " %s", req->to_string().c_str());
                req->request_again();
                return;
              }

              if(rsp.err == Error::RANGE_END_EARLIER) {
                locator->col_cells->add(*cells_buff.get(), rsp.range_end);
                auto next_key_start = locator->col_cells->get_key_next(rsp.range_end);
                if(next_key_start != nullptr) {
                  std::make_shared<Locator>(
                    locator->type, locator->cid, locator->col_cells, 
                    next_key_start, 
                    locator->updater, locator->parent
                  )->locate_on_manager();
                 }
              } else {
                locator->col_cells->add(*cells_buff.get());
                base->request_again();
              }
            }
            
            if(!--locator->updater->result->completion)
              locator->updater->response();
          },

          updater->timeout_commit += 
            cells_buff->fill()/updater->timeout_commit_bytes_ratio
        );

      } while(more);
    }

  };

};


}}}}}

#endif // swc_lib_db_protocol_common_req_QueryUpdate_h
