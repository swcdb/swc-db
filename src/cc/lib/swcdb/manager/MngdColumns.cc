
/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 */

#include "swcdb/manager/MngdColumns.h"
#include "swcdb/manager/Protocol/Mngr/req/ColumnUpdate.h"


namespace SWC { namespace Manager {



MngdColumns::MngdColumns()
    : m_run(true), m_schemas_mngr(false), m_columns_set(false), 
      cfg_schema_replication(Env::Config::settings()->get<Property::V_GUINT8>(
        "swc.mngr.schema.replication")),
      cfg_delay_cols_init(Env::Config::settings()->get<Property::V_GINT32>(
        "swc.mngr.ranges.assign.delay.afterColumnsInit")) { 
}

MngdColumns::~MngdColumns() {}

void MngdColumns::stop() {
  m_run = false;
}

bool MngdColumns::is_schemas_mngr(int& err) {
  if(m_schemas_mngr) {
    if(!m_columns_set)
      err = Error::MNGR_NOT_INITIALIZED; 
    return true;
  }
  return false;
}

void MngdColumns::active(const std::vector<cid_t>& cols) {
  if(!m_run)
    return;
  
  bool schemas_mngr = Env::Mngr::role()->is_active_role(
    Types::MngrRole::SCHEMAS);
  if(m_schemas_mngr && !schemas_mngr) {
    m_schemas_mngr = false;
    m_columns_set = false;
    SWC_LOG(LOG_INFO, "Manager(schemas) has been decommissioned");
  }

  {
    std::scoped_lock lock(m_mutex);
    if(cols.empty() && !m_cols_active.empty()) {
      if(!schemas_mngr)
        Env::Mngr::schemas()->reset();
      Env::Mngr::columns()->reset();
      Env::Mngr::rangers()->stop(false);
      m_cols_active.clear();
      SWC_LOG(LOG_INFO, "Manager(columns) has been decommissioned");
    }
    if(cols == m_cols_active && schemas_mngr == m_schemas_mngr)
      return;
    m_cols_active = cols;
  }

  Env::Mngr::rangers()->schedule_assignment_check(
    initialize() ? 500 : cfg_delay_cols_init->get());

  // if(m_schemas_mngr) (scheduled on column changes ) + chk(cid) LOAD_ACK
}

void MngdColumns::is_active(int& err, cid_t cid, bool for_schema) {
  if(!Env::Mngr::role()->is_active(cid)) {
    err = Error::MNGR_NOT_ACTIVE;
    return;
  }
  if((is_schemas_mngr(err) && err) || for_schema)
    return;

  Column::Ptr col = Env::Mngr::columns()->get_column(err, cid);
  if(col)
    col->state(err);
  else
    err = Error::COLUMN_NOT_EXISTS;
}

bool MngdColumns::has_active() {
  std::shared_lock lock(m_mutex);
  return !m_cols_active.empty();
}

void MngdColumns::require_sync() {
  Env::Mngr::rangers()->sync();
    
  if(m_schemas_mngr) {
    columns_load();
  } else {
    auto schema = DB::Schema::make();
    update(
      Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_LOAD_ALL, 
      schema
    );
  }
}

void MngdColumns::action(ColumnActionReq new_req) {
  {
    std::lock_guard lock(m_mutex_columns);
    m_actions.push(new_req);
    if(m_actions.size() > 1)
      return;
  }
  asio::post(*Env::IoCtx::io()->ptr(), [this]() { actions_run(); });
}

void MngdColumns::update_status(
                    Protocol::Mngr::Params::ColumnMng::Function func, 
                    DB::Schema::Ptr schema, int err, bool initial) {
  if(!initial && m_schemas_mngr)
    return update_status_ack(func, schema, err);

  err = Error::OK;
  if(!manage(schema->cid))
    return update(func, schema, err);

  switch(func) {

    case Protocol::Mngr::Params::ColumnMng::Function::DELETE: {
      remove(err, schema->cid);
      break;  
    } 

    case Protocol::Mngr::Params::ColumnMng::Function::CREATE:
    case Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_LOAD: {

      auto co_func = (
        Protocol::Mngr::Params::ColumnMng::Function)(((uint8_t)func)+1);

      if(Env::Mngr::columns()->is_an_initialization(err, schema) && !err) {
        {
          std::lock_guard lock(m_mutex_columns);
          m_cid_pending_load.emplace_back(co_func, schema->cid);
        }
        if(!m_schemas_mngr)
          Env::Mngr::schemas()->replace(schema);
        Env::Mngr::rangers()->assign_ranges();

      } else if(m_schemas_mngr) {
        update_status(co_func, schema, err);

      } else {
        update(co_func, schema, err);
      }
      break;
    }

    case Protocol::Mngr::Params::ColumnMng::Function::MODIFY: {
      if(m_schemas_mngr) {
        if(!Env::Mngr::rangers()->update(schema, true))
          update_status(
            Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_ACK_MODIFY, 
            schema, err);

      } else if(!update(schema)) {
        update(
          Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_ACK_MODIFY, 
          schema, err);
      }
    }
  }
}

void MngdColumns::load_pending(cid_t cid) {
  ColumnFunction pending;
  while(load_pending(cid, pending))
    update(pending.func, Env::Mngr::schemas()->get(pending.cid));
}

void MngdColumns::remove(int &err, cid_t cid, rgrid_t rgrid) {
  Column::Ptr col = Env::Mngr::columns()->get_column(err, cid);
  if(!col || col->finalize_remove(err, rgrid)) {
    Env::Mngr::columns()->remove(err, cid);
    DB::Schema::Ptr schema = Env::Mngr::schemas()->get(cid);
    if(schema)
      update(
        Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_ACK_DELETE,
        schema, err);
    if(!m_schemas_mngr)
      Env::Mngr::schemas()->remove(cid);
  }
}


std::string MngdColumns::to_string() {
  std::string s(Env::Mngr::columns()->to_string());
  return s;
}

bool MngdColumns::manage(cid_t cid) {
  std::shared_lock lock(m_mutex);

  if(m_cols_active.empty()) 
    return false; 

  if(*m_cols_active.begin() == 0 && *(m_cols_active.end()-1) < cid) 
    // from till-end
    return true;

  return std::find_if(m_cols_active.begin(), m_cols_active.end(),  
                      [cid](const cid_t& cid_set) 
                      {return cid_set == cid;}) != m_cols_active.end();
}

bool MngdColumns::initialize() {
  if(m_columns_set) {
    if(m_schemas_mngr)
      columns_load_chk_ack();
    return true;
  }
  
  m_schemas_mngr = Env::Mngr::role()->is_active_role(
    Types::MngrRole::SCHEMAS);
  if(!m_schemas_mngr || m_columns_set)
    return true;

  {
    std::scoped_lock lock1(m_mutex);
    std::scoped_lock lock2(m_mutex_columns);
    
    int err = Error::OK;
    FS::IdEntries_t entries;
    Columns::columns_by_fs(err, entries); 
    if(err) {
      if(err != ENOENT)
        return false;
      err = Error::OK;
    }
    if(entries.empty()) { // initialize sys-columns
      for(cid_t cid=1; cid <= Files::Schema::SYS_CID_END; ++cid) {
        Column::create(err, cid);
        entries.push_back(cid);
      }
    }
    
    int32_t hdlrs = Env::IoCtx::io()->get_size()/4+1;
    int32_t vol = entries.size()/hdlrs+1;
    std::atomic<int64_t> pending = 0;
    while(!entries.empty()) {
      FS::IdEntries_t hdlr_entries;
      for(auto n=0; n < vol && !entries.empty(); ++n) {
        hdlr_entries.push_back(entries.front());
        entries.erase(entries.begin());
      }
      if(hdlr_entries.empty())
        break;

      ++pending;
      asio::post(*Env::IoCtx::io()->ptr(), 
        [&pending, entries=hdlr_entries, 
         replicas=cfg_schema_replication->get()]() { 
          DB::Schema::Ptr schema;
          int err;
          for(cid_t cid : entries) {
            schema = Files::Schema::load(err = Error::OK, cid, replicas);
            if(!err)
              Env::Mngr::schemas()->add(err, schema);
            else
              SWC_LOGF(LOG_ERROR, "Schema cid=%d err=%d(%s)", 
                       cid, err, Error::get_text(err));
          }
          --pending;
        }
      );
    }

    while(pending) // keep_locking
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  
  m_columns_set = true;
  columns_load();
  return true;
}


void MngdColumns::columns_load() {
  std::vector<DB::Schema::Ptr> entries;
  Env::Mngr::schemas()->all(entries);
  for(auto& schema : entries) {
    SWC_ASSERT(schema->cid != DB::Schema::NO_CID);
    update_status(Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_LOAD, schema,
                  Error::OK, true);
  }
}

void MngdColumns::columns_load_chk_ack() {
  std::lock_guard lock(m_mutex_columns);
  for(auto& ack : m_cid_pending_load) {
    if(ack.func == Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_ACK_LOAD) {
      update(
        Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_LOAD,  
        Env::Mngr::schemas()->get(ack.cid)
      );
    }
  }
}

bool MngdColumns::load_pending(cid_t cid, ColumnFunction &pending) {
  std::lock_guard lock(m_mutex_columns);

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

cid_t MngdColumns::get_next_cid() {
  cid_t cid = Files::Schema::SYS_CID_END;
  while(++cid && Env::Mngr::schemas()->get(cid));
  // if schema does exist on fs (? sanity-check) 
  return cid; // err !cid
}

void MngdColumns::create(int &err, DB::Schema::Ptr &schema) {
  cid_t cid = get_next_cid();
  if(!cid) {
    err = Error::COLUMN_REACHED_ID_LIMIT;
    return;
  } 
  if(schema->col_seq == Types::KeySeq::UNKNOWN || 
     schema->col_type == Types::Column::UNKNOWN) {
    err = Error::INVALID_ARGUMENT;
    return;
  }
  
  Column::create(err, cid);
  if(err)
    return;

  if(Types::is_counter(schema->col_type))
    schema->cell_versions = 1;

  auto schema_save = DB::Schema::make(schema);
  schema_save->cid = cid;
  if(!schema_save->revision)
    schema_save->revision = Time::now_ns();
    
  Files::Schema::save_with_validation(
    err, schema_save, cfg_schema_replication->get());
  if(!err) 
    Env::Mngr::schemas()->add(err, schema_save);

  if(!err) 
    schema = Env::Mngr::schemas()->get(schema_save->cid);
  else 
    Column::remove(err, cid);
}
  
void MngdColumns::update(int &err, DB::Schema::Ptr &schema, 
                         DB::Schema::Ptr old) {
  if(old->col_seq != schema->col_seq || 
     Types::is_counter(old->col_type) != Types::is_counter(schema->col_type) ||
     (schema->cid <= Files::Schema::SYS_CID_END && 
      schema->col_name.compare(old->col_name) != 0)) {
    err = Error::COLUMN_CHANGE_INCOMPATIBLE;
    return;
  }

  if(Types::is_counter(schema->col_type))
    schema->cell_versions = 1;

  if(schema->cid < Files::Schema::SYS_CID_END) { 
    //different values bad for range-colms
    schema->cell_versions = 1;
    schema->cell_ttl = 0;
  }

  auto schema_save = DB::Schema::make(schema);
  if(schema_save->cid == DB::Schema::NO_CID)
    schema_save->cid = old->cid;
  if(!schema_save->revision)
    schema_save->revision = Time::now_ns();

  if(schema_save->cid == DB::Schema::NO_CID)
    err = Error::COLUMN_SCHEMA_ID_EMPTY;
  if(schema_save->equal(old, false))
    err = Error::COLUMN_SCHEMA_NOT_DIFFERENT;
  if(err)
    return;

  Files::Schema::save_with_validation(
    err, schema_save, cfg_schema_replication->get());
  if(!err) {
    Env::Mngr::schemas()->replace(schema_save);
    schema = Env::Mngr::schemas()->get(schema_save->cid);
  }
}

void MngdColumns::remove(int &err, cid_t cid) {
  Column::Ptr col = Env::Mngr::columns()->get_column(err, cid);
  if(!col)
    return remove(err, cid, 0);
  if(!col->do_remove())
    return;
  SWC_LOGF(LOG_DEBUG, "DELETING cid=%d", cid);
    
  std::vector<rgrid_t> rgrids;
  col->assigned(rgrids);
  if(rgrids.empty())
    return remove(err, cid, 0);

  Env::Mngr::rangers()->column_delete(cid, rgrids);
}

bool MngdColumns::update(DB::Schema::Ptr schema) {
  DB::Schema::Ptr existing = Env::Mngr::schemas()->get(schema->cid);
  if(!existing || !existing->equal(schema)) {
    Env::Mngr::schemas()->replace(schema);
    return Env::Mngr::rangers()->update(schema, true);
  }
  return false;
}


void MngdColumns::update(Protocol::Mngr::Params::ColumnMng::Function func,
                         DB::Schema::Ptr schema, int err) {
  Env::Mngr::role()->req_mngr_inchain(
    std::make_shared<Protocol::Mngr::Req::ColumnUpdate>(func, schema, err));
}

void MngdColumns::update_status_ack(
                          Protocol::Mngr::Params::ColumnMng::Function func,
                          DB::Schema::Ptr schema, int err) {
  if(!err) switch(func) {
    case Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_LOAD_ALL: {
      return columns_load();
    }
    case Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_ACK_LOAD: {
      ColumnFunction pending;
      while(load_pending(schema->cid, pending));
      return;
    }
    case Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_ACK_CREATE:
    case Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_ACK_MODIFY:
      break;
    case Protocol::Mngr::Params::ColumnMng::Function::INTERNAL_ACK_DELETE: {
      std::scoped_lock lock(m_mutex);

      if(!Env::Mngr::schemas()->get(schema->cid)) {
        err = Error::COLUMN_SCHEMA_NAME_NOT_EXISTS;
      } else if(!m_run) {
        err = Error::SERVER_SHUTTING_DOWN;
      } else {
        Column::remove(err, schema->cid);
        if(!err)
          Env::Mngr::schemas()->remove(schema->cid);
      }
      break;
    }
    default:
      return;
  }

  auto co_func = (Protocol::Mngr::Params::ColumnMng::Function)(((uint8_t)func)-1);

  if(err)
    SWC_LOGF(LOG_DEBUG, "COLUMN-ACK %s func=%d, err=%d(%s)", 
              schema->to_string().c_str(), co_func, 
              err, Error::get_text(err));
                  
  ColumnActionReq req;
  for(;;) {
    {
      std::lock_guard lock(m_mutex_columns);
      auto it = std::find_if(m_cid_pending.begin(), m_cid_pending.end(),  
          [co_func, cid=schema->cid](const ColumnActionReq& req)
          {return req.params.schema->cid == cid 
                  && req.params.function == co_func;});
      if(it == m_cid_pending.end())
        break;  
      req = *it;
      m_cid_pending.erase(it);
    }

    try {
      req.cb(err);
    } catch (std::exception &e) {
      SWC_LOGF(LOG_ERROR, "Column Pending func=%d cb err=%s %s", 
                func, e.what(), req.params.schema->to_string().c_str());
    }
  }
}

void MngdColumns::actions_run() {  
  ColumnActionReq req;
  int err;
  for(;;) {
    {
      std::lock_guard lock(m_mutex_columns);
      req = m_actions.front();
    }

    if(!m_run) {
      err = Error::SERVER_SHUTTING_DOWN;

    } else if(!m_columns_set) {
      err = Error::MNGR_NOT_INITIALIZED;

    } else if(req.params.schema->col_name.empty()) {
      err = Error::COLUMN_SCHEMA_NAME_EMPTY;

    } else {
      err = Error::OK;
      std::scoped_lock lock(m_mutex);
      DB::Schema::Ptr schema = Env::Mngr::schemas()->get(
        req.params.schema->col_name);
      if(!schema && req.params.schema->cid != DB::Schema::NO_CID)
        schema = Env::Mngr::schemas()->get(req.params.schema->cid);

      switch(req.params.function) {
        case Protocol::Mngr::Params::ColumnMng::Function::CREATE: {
          if(schema)
            err = Error::COLUMN_SCHEMA_NAME_EXISTS;
          else
            create(err, req.params.schema);  
          break;
        }
        case Protocol::Mngr::Params::ColumnMng::Function::MODIFY: {
          if(!schema) 
            err = Error::COLUMN_SCHEMA_NAME_NOT_EXISTS;
          else if(req.params.schema->cid != DB::Schema::NO_CID && 
                  schema->cid != req.params.schema->cid)
            err = Error::COLUMN_SCHEMA_NAME_EXISTS;
          else
            update(err, req.params.schema, schema);
          break;
        }
        case Protocol::Mngr::Params::ColumnMng::Function::DELETE: {
          if(!schema)
            err = Error::COLUMN_SCHEMA_NAME_NOT_EXISTS;
          else if(schema->cid != req.params.schema->cid ||
                  req.params.schema->col_name.compare(schema->col_name) != 0)
            err = Error::COLUMN_SCHEMA_NAME_NOT_CORRES;
          else if(schema->cid <= Files::Schema::SYS_CID_END)
            err = Error::COLUMN_SCHEMA_IS_SYSTEM;
          else
            req.params.schema = schema;
          break;
        }
        default:
          err = Error::NOT_IMPLEMENTED;
          break;
      }
    }

    if(!err) {
      {
        std::lock_guard lock(m_mutex_columns);
        m_cid_pending.push_back(req);
      }
      SWC_ASSERT(req.params.schema->cid != DB::Schema::NO_CID);
      update_status(req.params.function, req.params.schema, err, true);

    } else {
      try{
        req.cb(err);
      } catch (std::exception &e) {
        SWC_LOGF(LOG_ERROR, "Column Action cb err=%s func=%d %s", 
                  e.what(), req.params.function, 
                  req.params.schema->to_string().c_str());
      }
    }
      
    {
      std::lock_guard lock(m_mutex_columns);
      m_actions.pop();
      if(m_actions.empty())
        return;
    }
  }
}



}} // namespace Manager