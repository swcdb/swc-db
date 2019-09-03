
/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */

#ifndef swc_lib_manager_RangeServers_h
#define swc_lib_manager_RangeServers_h
#include <memory>

#include "swcdb/lib/db/Columns/MNGR/Columns.h"

#include "swcdb/lib/db/Files/RsData.h"
#include "RsStatus.h"

#include "swcdb/lib/db/Files/Schema.h"
#include "swcdb/lib/db/Protocol/params/MngColumn.h"

#include "swcdb/lib/db/Protocol/req/LoadRange.h"
#include "swcdb/lib/db/Protocol/req/IsRangeLoaded.h"
#include "swcdb/lib/db/Protocol/req/RsIdReqNeeded.h"
#include "swcdb/lib/db/Protocol/req/MngrUpdateRangeServers.h"
#include "swcdb/lib/db/Protocol/req/MngrUpdateColumn.h"
#include "swcdb/lib/db/Protocol/req/RsColumnDelete.h"


namespace SWC { namespace server { namespace Mngr {

class RangeServers;
typedef std::shared_ptr<RangeServers> RangeServersPtr;
}}

namespace Env {
class RangeServers {
  
  public:

  static void init() {
    m_env = std::make_shared<RangeServers>();
  }

  static server::Mngr::RangeServersPtr get(){
    HT_ASSERT(m_env != nullptr);
    return m_env->m_rangeservers;
  }

  RangeServers() 
    : m_rangeservers(std::make_shared<server::Mngr::RangeServers>()) {}

  virtual ~RangeServers(){}

  private:
  server::Mngr::RangeServersPtr               m_rangeservers = nullptr;
  inline static std::shared_ptr<RangeServers> m_env = nullptr;
};
}

namespace server { namespace Mngr {


class RangeServers {

  struct ColumnActionReq {
    Protocol::Params::MngColumn params;
    std::function<void(int)>    cb;
  };
  struct ColumnFunction {
    Protocol::Params::MngColumn::Function func;
    int64_t  cid;
  };

  public:
  RangeServers()
    : m_assign_timer(
        std::make_shared<asio::high_resolution_timer>(*Env::IoCtx::io()->ptr())
      ),
      cfg_rs_failures(Env::Config::settings()->get_ptr<gInt32t>(
        "swc.mngr.ranges.assign.RS.remove.failures")),
      cfg_delay_rs_chg(Env::Config::settings()->get_ptr<gInt32t>(
        "swc.mngr.ranges.assign.delay.onRangeServerChange")),
      cfg_delay_cols_init(Env::Config::settings()->get_ptr<gInt32t>(
        "swc.mngr.ranges.assign.delay.afterColumnsInit")),
      cfg_chk_assign(Env::Config::settings()->get_ptr<gInt32t>(
        "swc.mngr.ranges.assign.interval.check")) {
  }

  void new_columns() {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_columns_set = false;
    }
    check_assignment_timer(500);
  }
  
  void require_sync() {
    rs_changes(m_rs_status, true);
  }

  virtual ~RangeServers(){}


  // Columns Actions
  void column_action(ColumnActionReq new_req){
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_actions.push(new_req);
      if(m_actions.size() > 1)
        return;
    }
    
    ColumnActionReq req;
    int err;
    for(;;){
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        req = m_actions.front();
        err = m_columns_set ? Error::OK : Error::MNGR_NOT_INITIALIZED;
      }

      if(err == Error::OK){
        DB::SchemaPtr schema = Env::Schemas::get()->get(req.params.schema->col_name);

        switch(req.params.function){
          case Protocol::Params::MngColumn::Function::CREATE: {
            if(schema != nullptr)
              err = Error::SCHEMA_COL_NAME_EXISTS;
            else
              column_create(req.params.schema, err);  
            break;
          }
          case Protocol::Params::MngColumn::Function::DELETE: {
            if(schema == nullptr) 
              err = Error::SCHEMA_COL_NAME_NOT_EXISTS;
            else 
              req.params.schema = schema;
            break;
          }
          default:
            err = Error::NOT_IMPLEMENTED;
            break;
        }
      }

      if(err == Error::OK){
        update_status(req.params.function, req.params.schema->cid, true);
        {
          std::lock_guard<std::mutex> lock(m_mutex);
          m_cid_pending.push_back(req);
        }

      } else {
        try{
          req.cb(err);
        } catch (std::exception &e) {
          HT_ERRORF("Column Action cb err=%s func=%d %s", 
                    e.what(), req.params.function, 
                    req.params.schema->to_string().c_str());
        }
      }
      
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_actions.pop();
        if(m_actions.empty())
          return;
      }
    }
  }

  void update_status(Protocol::Params::MngColumn::Function func, int64_t cid, 
                     bool initial=false){
    HT_ASSERT(cid != 0);

    if(!initial && m_root_mngr) {
      bool processed = true;
      int err = Error::OK;
      Protocol::Params::MngColumn::Function co_func;
      
      switch(func){
        
        case Protocol::Params::MngColumn::Function::INTERNAL_ACK_LOAD: {
          ColumnFunction pending;
          while(column_pending_load(cid, pending));
          return;
        }
        case Protocol::Params::MngColumn::Function::INTERNAL_ACK_CREATE: {
          co_func = Protocol::Params::MngColumn::Function::CREATE;
          break;
        }
        case Protocol::Params::MngColumn::Function::INTERNAL_ACK_DELETE: {
          co_func = Protocol::Params::MngColumn::Function::DELETE;
          if(Env::Schemas::get()->get(cid) == nullptr)
            err = Error::SCHEMA_COL_NAME_NOT_EXISTS;
          else {
            Env::Schemas::get()->remove(cid);  
            Column::remove(cid); //Files::Schema::remove(cid);
            {
              std::lock_guard<std::mutex> lock(m_mutex);
              m_cols_reuse.push(cid);
            }
          }
          break;
        }
        default:{
          processed = false;
        }
      }

      if(processed){
        ColumnActionReq req;
        for(;;){
          {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = std::find_if(m_cid_pending.begin(), m_cid_pending.end(),  
              [co_func, cid](const ColumnActionReq& req)
              {return req.params.schema->cid == cid 
                      && req.params.function == co_func;});
            
            if(it == m_cid_pending.end())
              break;  
            req = *it;
            m_cid_pending.erase(it);
          }

          try{
            req.cb(err);
          } catch (std::exception &e) {
            HT_ERRORF("Column Pending func=%d cb err=%s %s", 
                      func, e.what(), req.params.schema->to_string().c_str());
          }
        }
        return;
      }
    }

    if(manage(cid)){
      switch(func){
        case Protocol::Params::MngColumn::Function::INTERNAL_LOAD: {
          Env::MngrColumns::get()->get_column(cid, true);
          {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_cid_pending_load.push_back({
              .func=Protocol::Params::MngColumn::Function::INTERNAL_ACK_LOAD, 
              .cid=cid});
          }
          check_assignment();
          break;
        }
        case Protocol::Params::MngColumn::Function::CREATE: {
          Env::MngrColumns::get()->get_column(cid, true);
          {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_cid_pending_load.push_back({
              .func=Protocol::Params::MngColumn::Function::INTERNAL_ACK_CREATE, 
              .cid=cid});
          }
          check_assignment();
          break;
        }
        case Protocol::Params::MngColumn::Function::DELETE: {
          column_delete(cid);
          break;
        }
        default:
          break;
      }
      return;
    }
    
    Protocol::Req::MngrUpdateColumn::put(func, cid);
  }

  // RangeServer Actions
  void update_status(RsStatusList new_rs_status, bool sync_all){
    if(m_root_mngr && !sync_all)
      return;

    RsStatusList changed;

    {
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);
      RsStatusPtr h;
      bool found;
      bool chg;

      for(auto& rs_new : new_rs_status){
        found = false;
        for(auto it=m_rs_status.begin();it<m_rs_status.end(); it++){
          h = *it;
          if(!has_endpoint(h->endpoints, rs_new->endpoints))
            continue;

          chg = false;
          if(rs_new->rs_id != h->rs_id){ 
            if(m_root_mngr)
              rs_new->rs_id = rs_set(rs_new->endpoints, rs_new->rs_id)->rs_id;
          
            if(m_root_mngr && rs_new->rs_id != h->rs_id)
              Env::MngrColumns::get()->change_rs(h->rs_id, rs_new->rs_id);

            h->rs_id = rs_new->rs_id;
            chg = true;
          }
          for(auto& endpoint: rs_new->endpoints){
            if(!has_endpoint(endpoint, h->endpoints)){
              h->set(rs_new->endpoints);
              chg = true;
              break;
            }
          }
          for(auto& endpoint: h->endpoints){
            if(!has_endpoint(endpoint, rs_new->endpoints)){
              h->set(rs_new->endpoints);
              chg = true;
              break;
            }
          }
          if(rs_new->state == RsStatus::State::ACK) {
            if(rs_new->state != h->state) {
              h->state = RsStatus::State::ACK;
              chg = true;
            }
          } else {
            Env::MngrColumns::get()->set_rs_unassigned(h->rs_id);
            m_rs_status.erase(it);
            chg = true;
          }

          if(chg && !sync_all)
            changed.push_back(rs_new);
          found = true;
          break;
        }

        if(!found){
          if(rs_new->state == RsStatus::State::ACK){
            rs_new->init_queue();
            m_rs_status.push_back(rs_new);
            if(!sync_all)
              changed.push_back(rs_new);
          }
        }
      }
    }
    if(!changed.empty())
      std::cout << " update_status: ";

    rs_changes(sync_all ? m_rs_status : changed, sync_all && !m_root_mngr);
  }

  uint64_t rs_set_id(const EndPoints& endpoints, uint64_t opt_id=0){
    std::lock_guard<std::mutex> lock(m_mutex_rs_status);
    return rs_set(endpoints, opt_id)->rs_id;
  }

  bool rs_ack_id(uint64_t rs_id, const EndPoints& endpoints){
    bool ack = false;
    RsStatusPtr new_ack = nullptr;
    {
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);
    
      for(auto& h : m_rs_status){
        if(has_endpoint(h->endpoints, endpoints) && rs_id == h->rs_id){
          if(h->state != RsStatus::State::ACK)
            new_ack = h;
          h->state = RsStatus::State::ACK;
          ack = true;
          break;
        }
      }
    }

    if(new_ack != nullptr) 
      rs_changes({new_ack});
    return ack;
  }

  uint64_t rs_had_id(uint64_t rs_id, const EndPoints& endpoints){
    bool new_id_required = false;
    {
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);

      for(auto& h : m_rs_status){
        if(rs_id == h->rs_id){
          if(has_endpoint(h->endpoints, endpoints))
            return 0; // zero=OK
          new_id_required = true;
          break;
        }
      }
    }
    
    return rs_set_id(endpoints, new_id_required ? 0 : rs_id);
  }

  void rs_shutdown(uint64_t rs_id, const EndPoints& endpoints){
    RsStatusPtr removed = nullptr;
    {
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);
      for(auto it=m_rs_status.begin();it<m_rs_status.end(); it++){
        auto h = *it;
        if(has_endpoint(h->endpoints, endpoints)){
          removed = h;
          m_rs_status.erase(it);
          removed->state = RsStatus::State::REMOVED;
          Env::MngrColumns::get()->set_rs_unassigned(removed->rs_id);
          break;
        }
      }
    }
    if(removed != nullptr)
      rs_changes({removed});   
  }

  std::string to_string(){
    std::string s(Env::MngrColumns::get()->to_string());

    s.append("\nRangeServers:");
    std::lock_guard<std::mutex> lock(m_mutex_rs_status);
    for(auto& h : m_rs_status) {
      s.append("\n ");
      s.append(h->to_string());
    }
    return s;
  }

  void stop() {
    {
      std::lock_guard<std::mutex> lock(m_mutex_timer);
      m_assign_timer->cancel();
      m_run = false;
    }
    {
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);
      for(auto& h : m_rs_status)
        asio::post(*Env::IoCtx::io()->ptr(), [h](){ h->stop(); });
    }
  }

  private:

  bool manage(int64_t cid){
    std::vector<int64_t> cols;
    Env::MngrRoleState::get()->get_active_columns(cols);
    if(cols.size() == 0){
      // if decommissioned
      std::lock_guard<std::mutex> lock(m_mutex);
      if(m_columns_set){
        HT_INFO("Manager has been decommissioned");
        m_columns_set = false;
        m_root_mngr = false;
        m_last_cid = 0;
        Env::MngrColumns::get()->reset();
        while(!m_cols_reuse.empty())
          m_cols_reuse.pop();
      }
      return false; 
    }

    if(*cols.begin() == 0 && *(cols.end()-1) < cid) // from till-end
      return true;

    return std::find_if(cols.begin(), cols.end(),  
          [cid](const int64_t& cid_set){return cid_set == cid;}) != cols.end();
  }

  void check_assignment(){
     if(!initialize_cols()){
      check_assignment_timer(cfg_delay_cols_init->get());
      return;
    }

    // if(m_root_mngr) (scheduled on column changes ) + chk(cid) LOAD_ACK

    int64_t assigned = 0;
    assign_ranges(assigned);
    if(assigned == -1) {
      check_assignment_timer(10000);
      return;
    } else if(assigned == -2)
      return;
    std::cout << "  assigned=" << assigned << "\n";

    // for rangeserver cid-rid state

    check_assignment_timer(cfg_chk_assign->get());
  }

  void check_assignment_timer(uint32_t t_ms = 10000) {

    std::lock_guard<std::mutex> lock(m_mutex_timer);
    if(!m_run)
      return;

    auto set_in = std::chrono::milliseconds(t_ms);
    auto set_on = m_assign_timer->expires_from_now();
    if(set_on > std::chrono::milliseconds(0) && set_on < set_in)
      return;
    m_assign_timer->cancel();
    m_assign_timer->expires_from_now(set_in);

    m_assign_timer->async_wait(
      [](const asio::error_code ec) {
        if (ec != asio::error::operation_aborted){
          Env::RangeServers::get()->check_assignment();
        }
    }); 

    if(t_ms > 10000) {
      std::cout << to_string() << "\n";
    }
    HT_DEBUGF("RS ranges check_assignment scheduled in ms=%d", t_ms);
  }

  bool initialize_cols(){
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      if(m_columns_set){
        if(m_root_mngr)
          chk_columns_load_ack();
        return true;
      }
    }
    m_root_mngr = manage(1);
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<int64_t> cols;
    Env::MngrRoleState::get()->get_active_columns(cols);
    if(cols.size() == 0){
      m_columns_set = false;
      return false; 
    }

    if(!m_root_mngr || m_last_cid > 0){
      m_columns_set = true;
      return true;
    }

    int err = Error::OK;
    FS::IdEntries_t entries;

    Columns::columns_by_fs(err, entries); 
    if(err !=  Error::OK) {
      if(err != ENOENT)
        return false;
    }
    
    if(entries.empty()){
      // initialize sys-columns
      for(int cid=1;cid<=3;cid++){
        Column::create(cid);
        Env::Schemas::get()->add(Files::Schema::load(cid));
        Env::MngrColumns::get()->get_column(cid, true);
      }
      m_last_cid = 3;

    } else {
      for(auto cid : entries){
        Env::Schemas::get()->add(Files::Schema::load(cid));
        if(manage(cid))
          Env::MngrColumns::get()->get_column(cid, true);
        else {
          Protocol::Req::MngrUpdateColumn::put(
            Protocol::Params::MngColumn::Function::INTERNAL_LOAD, cid);
          
          m_cid_pending_load.push_back({
            .func=Protocol::Params::MngColumn::Function::INTERNAL_ACK_LOAD, 
            .cid=cid});
        }
        if(cid > m_last_cid)
          m_last_cid = cid;
      }
      for(int cid=3;cid<=m_last_cid;cid++){
        if(Env::Schemas::get()->get(cid) == nullptr)
          m_cols_reuse.push(cid);
      }
    }

    m_columns_set = true;
    return true;
  }
  
  void assign_ranges(int64_t &assigned) {
    {
      std::lock_guard<std::mutex> lock(m_mutex_assign);
      if(m_runs_assign) {
        assigned = -2;
        return;
      }
      m_runs_assign = true;
    }

    RangePtr range;

    for(;;){
      std::lock_guard<std::mutex> lock(m_mutex_assign);
      {
        std::lock_guard<std::mutex> lock(m_mutex_rs_status);
        if(m_rs_status.empty()){
          m_runs_assign = false;
          assigned = -1;
          return;
        }
      }

      if((range = Env::MngrColumns::get()->get_next_unassigned()) == nullptr){
        m_runs_assign = false;
        return;
      }
      
      Files::RsDataPtr last_rs = range->get_last_rs();
      RsStatusPtr rs = nullptr;
      next_rs(last_rs, rs);
      if(rs == nullptr){
        m_runs_assign = false;
        assigned = -1;
        return;
      }

      range->set_state(Range::State::QUEUED, rs->rs_id);
      assign_range(rs, range, last_rs);
      assigned++;
    }
  }

  void next_rs(Files::RsDataPtr &last_rs, RsStatusPtr &rs_set){
    std::lock_guard<std::mutex> lock(m_mutex_rs_status);

    if(last_rs->endpoints.size() > 0) {
       for(auto& rs : m_rs_status) {
          if(rs->state == RsStatus::State::ACK
            && rs->failures < cfg_rs_failures->get() 
            && has_endpoint(rs->endpoints, last_rs->endpoints)){
            rs_set = rs;
            last_rs = nullptr;
            break;
          }
       }
    } else 
      last_rs = nullptr;
    
    size_t num_rs;
    size_t avg_ranges;
    RsStatusPtr rs;

    while(rs_set == nullptr && m_rs_status.size() > 0){
      avg_ranges = 0;
      num_rs = 0;
      // avg_resource_ratio = 0;
      for(auto it=m_rs_status.begin();it<m_rs_status.end(); it++) {
        rs = *it;
        if(rs->state != RsStatus::State::ACK)
          continue;
        avg_ranges = avg_ranges*num_rs + rs->total_ranges;
        // resource_ratio = avg_resource_ratio*num_rs + rs->resource();
        avg_ranges /= ++num_rs;
        // avg_resource_ratio /= num_rs;
      }

      for(auto it=m_rs_status.begin();it<m_rs_status.end(); it++){
        rs = *it;
        if(rs->state != RsStatus::State::ACK || avg_ranges < rs->total_ranges)
          continue;

        if(rs->failures == cfg_rs_failures->get()){
          m_rs_status.erase(it);
          Env::MngrColumns::get()->set_rs_unassigned(rs->rs_id);
          continue;
        }
        rs_set = rs;
        break;
      }
    }

    if(rs_set != nullptr)
      rs_set->total_ranges++;
    return;
  }

  void assign_range(RsStatusPtr rs, RangePtr range, 
                    Files::RsDataPtr last_rs){
    if(last_rs == nullptr){
      assign_range(rs, range);
      return;
    }

    RsStatusPtr rs_last = nullptr;
    {
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);
      for(auto& rs_chk : m_rs_status) {
        if(has_endpoint(rs_chk->endpoints, last_rs->endpoints)){
          rs_last = rs_chk;
          break;
        }
      }
    }
    if(rs_last == nullptr){
      rs_last = std::make_shared<RsStatus>(0, last_rs->endpoints);
      rs_last->init_queue();
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);
      std::cout <<  " assign_range, rs_last " << rs_last->to_string() << "\n";
      m_rs_status.push_back(rs_last);
    }

    rs_last->put(
      [rs, range](client::ClientConPtr conn){
        if(conn == nullptr || !(std::make_shared<Protocol::Req::RsIdReqNeeded>(
          conn, [rs, range](bool err) {     
            err ? Env::RangeServers::get()->assign_range(rs, range)
                : Env::RangeServers::get()->range_loaded(rs, range, false);
          }))->run()) {
          Env::RangeServers::get()->assign_range(rs, range);
        }
      }
    );
  }

  void assign_range(RsStatusPtr rs, RangePtr range){
    rs->put(
      [rs, range](client::ClientConPtr conn){
        if(range->deleted()
          || conn == nullptr 
          || !(std::make_shared<Protocol::Req::LoadRange>(
              conn, range, 
              [rs, range](bool loaded){
                Env::RangeServers::get()->range_loaded(rs, range, loaded); 
              }
          ))->run()) {
          Env::RangeServers::get()->range_loaded(rs, range, false, true);
        }
      }
    );
  }

  void range_loaded(RsStatusPtr rs, RangePtr range, 
                    bool loaded, bool failure=false) { // + resource_chg
    if(range->deleted())
      return;

    if(!loaded){
      {
        std::lock_guard<std::mutex> lock(m_mutex_rs_status);
        rs->total_ranges--;
        if(failure)
          rs->failures++;
      }
      range->set_state(Range::State::NOTSET, 0); 
      check_assignment_timer(2000);

    } else {
      {
        std::lock_guard<std::mutex> lock(m_mutex_rs_status);
        rs->failures=0;
      }
      range->set_state(Range::State::ASSIGNED, rs->rs_id); 
      // adjust rs->resource
      // ++ mng_inchain - req. MngrRsResource
      
      ColumnFunction pending;
      while(column_pending_load(range->cid, pending))
        Protocol::Req::MngrUpdateColumn::put(pending.func, pending.cid);
    }

    HT_DEBUGF("RANGE-STATUS, %s", range->to_string().c_str());
  }

  RsStatusPtr rs_set(const EndPoints& endpoints, uint64_t opt_id=0){

    for(auto it=m_rs_status.begin();it<m_rs_status.end(); it++){
      auto h = *it;
      if(has_endpoint(h->endpoints, endpoints)) {
        if(h->state == RsStatus::State::ACK) {
          h->set(endpoints);
          return h;
        } else {
          Env::MngrColumns::get()->set_rs_unassigned(h->rs_id);
          m_rs_status.erase(it);
          break;
        }
      }
    }

    uint64_t next_id=0;
    uint64_t nxt;
    bool ok;
    do {
      if(opt_id == 0) {
        nxt = ++next_id;
      } else {
        nxt = opt_id;
        opt_id = 0;
      }
      
      ok = true;
      for(auto& h : m_rs_status){
        if(nxt == h->rs_id) {
          ok = false;
          break;
        };
      }
    } while(!ok);

    RsStatusPtr h = std::make_shared<RsStatus>(nxt, endpoints);
    m_rs_status.push_back(h);
    return h;
  }
  
  void rs_changes(RsStatusList hosts, bool sync_all=false){
    {
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);
      if(hosts.size() > 0){
        Protocol::Req::MngrUpdateRangeServers::put(hosts, sync_all);
        std::cout << " rs_changes: \n";
        for(auto& h : hosts)
          std::cout << " " << h->to_string() << "\n";
      }
    }
    
    if(Env::MngrRoleState::get()->has_active_columns())
      check_assignment_timer(cfg_delay_rs_chg->get());
  }
  

  int64_t get_next_cid() {
    std::lock_guard<std::mutex> lock(m_mutex);

    while(m_cols_reuse.size() > 0) {
      int64_t cid = m_cols_reuse.front();
      m_cols_reuse.pop();
      if(Env::Schemas::get()->get(cid) == nullptr)
        return cid;
    }
    return ++m_last_cid;
  }

  void column_create(DB::SchemaPtr &schema, int &err){
    int64_t cid = get_next_cid();
    Column::create(cid);
      
    schema = DB::Schema::make(cid, schema);
    Env::Schemas::get()->add(schema);

    Files::Schema::save(schema);
  }

  void column_delete(int64_t cid) {

    ColumnPtr col = Env::MngrColumns::get()->get_column(cid, false);
    if(col == nullptr){
      column_delete(cid, 0);
      return;
    }
    if(!col->do_remove())
      return;
    HT_DEBUGF("DELETING cid=%d", cid);
    
    std::vector<uint64_t> rs_ids;
    col->assigned(rs_ids);
    if(rs_ids.empty()){
      column_delete(cid, 0);
      return;
    }

    for(auto rs_id : rs_ids){
      std::lock_guard<std::mutex> lock(m_mutex_rs_status);
      for(auto& rs : m_rs_status){
        if(rs_id != rs->rs_id)
          continue;
        rs->total_ranges--; 

        RsQueue::ConnCb_t cb;
        cb = [rs, cid, cb = &cb](client::ClientConPtr conn){
          if(conn == nullptr || !(
             std::make_shared<Protocol::Req::RsColumnDelete>(conn, cid, 
              [rs, cid, cb](bool ok){
                if(ok)  Env::RangeServers::get()->column_delete(cid, rs->rs_id);  
                else    rs->put(*cb);
              }
          ))->run()) {
            Env::RangeServers::get()->column_delete(cid, rs->rs_id);
          }
        };
        rs->put(cb);
      }
    }
  }

  void column_delete(int64_t cid, int64_t rs_id) {
    ColumnPtr col = Env::MngrColumns::get()->get_column(cid, false);
    if(col == nullptr || col->finalize_remove(rs_id)) {
      Env::MngrColumns::get()->remove(cid);
      Protocol::Req::MngrUpdateColumn::put(
        Protocol::Params::MngColumn::Function::INTERNAL_ACK_DELETE, cid);
    }
  }
         
  bool column_pending_load(int64_t cid, ColumnFunction &pending) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = std::find_if(m_cid_pending_load.begin(), 
                           m_cid_pending_load.end(),  
                          [cid](const ColumnFunction& pending) 
                          {return pending.cid == cid;});
    if(it == m_cid_pending_load.end())
      return false;
    pending = *it;
    m_cid_pending_load.erase(it);
    return true;
  }

  void chk_columns_load_ack(){
    for(auto& pending : m_cid_pending_load){
      if(pending.func == Protocol::Params::MngColumn::Function::INTERNAL_ACK_LOAD){
        Protocol::Req::MngrUpdateColumn::put(
          Protocol::Params::MngColumn::Function::INTERNAL_LOAD, pending.cid);
      }
    }
  }


  std::mutex                    m_mutex_timer;
  TimerPtr                      m_assign_timer; 
  bool                          m_run=true; 

  std::mutex                    m_mutex;
  bool                          m_columns_set = false;
  int64_t                       m_last_cid = 0;
  std::queue<int64_t>           m_cols_reuse;
  std::queue<ColumnActionReq>   m_actions;
  std::vector<ColumnActionReq>  m_cid_pending;
  std::vector<ColumnFunction>   m_cid_pending_load;

  std::mutex                    m_mutex_rs_status;
  RsStatusList                  m_rs_status;

  std::mutex                    m_mutex_assign;
  bool                          m_runs_assign = false;

  std::atomic<bool>             m_root_mngr = false;
  

  const gInt32tPtr cfg_rs_failures;
  const gInt32tPtr cfg_delay_rs_chg;
  const gInt32tPtr cfg_delay_cols_init;
  const gInt32tPtr cfg_chk_assign;
  
};


}}}

#endif // swc_lib_manager_RangeServers_h