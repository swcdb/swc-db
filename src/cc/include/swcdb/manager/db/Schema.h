/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 */


#ifndef swc_manager_db_Schema_h
#define swc_manager_db_Schema_h

#include "swcdb/core/Checksum.h"
#include "swcdb/db/Types/MetaColumn.h"

namespace SWC { namespace Files { namespace Schema {

const uint8_t SYS_CID_END = 9;

const uint8_t HEADER_SIZE = 13;
const uint8_t HEADER_OFFSET_CHKSUM = 9;
const uint8_t VERSION = 1;
const std::string schema_file = "schema.data";

/* file-format: 
    header: i8(version), i32(data-len), 
            i32(data-checksum), i32(header-checksum)
    data:   schema-encoded
*/

std::string filepath(int64_t cid){
  std::string path = DB::RangeBase::get_column_path(cid);
  path.append("/");
  path.append(schema_file);
  return path;
}

// REMOVE

void remove(int &err, int64_t cid){
  Env::FsInterface::interface()->remove(err, filepath(cid));
}

// SET 

void write(SWC::DynamicBuffer &dst_buf, DB::Schema::Ptr schema){
    
  size_t sz = schema->encoded_length();
  dst_buf.ensure(HEADER_SIZE+sz);

  Serialization::encode_i8(&dst_buf.ptr, VERSION);
  Serialization::encode_i32(&dst_buf.ptr, sz);

  uint8_t* checksum_data_ptr = dst_buf.ptr;
  Serialization::encode_i32(&dst_buf.ptr, 0);
  uint8_t* checksum_header_ptr = dst_buf.ptr;
  Serialization::encode_i32(&dst_buf.ptr, 0);

  const uint8_t* start_data_ptr = dst_buf.ptr;
  schema->encode(&dst_buf.ptr);

  checksum_i32(start_data_ptr, dst_buf.ptr, &checksum_data_ptr);
  checksum_i32(dst_buf.base, start_data_ptr, &checksum_header_ptr);

  SWC_ASSERT(dst_buf.fill() <= dst_buf.size);
}

void save(int &err, DB::Schema::Ptr schema, uint8_t replication=0){
  DynamicBuffer input;
  write(input, schema);
  StaticBuffer send_buf(input);
  
  Env::FsInterface::interface()->write(
    err,
    FS::SmartFd::make_ptr(filepath(schema->cid), 
                          FS::OpenFlags::OPEN_FLAG_OVERWRITE), 
    replication,
    -1, 
    send_buf
  );
}


//  GET

void load(int &err, const std::string& filepath, DB::Schema::Ptr &schema) {
  
  StaticBuffer read_buf;
  Env::FsInterface::interface()->read(err, filepath, &read_buf);
  if(err) 
    return;
  
  const uint8_t *ptr = read_buf.base;
  size_t remain = read_buf.size;
  
  int8_t version = Serialization::decode_i8(&ptr, &remain);
  size_t sz = Serialization::decode_i32(&ptr, &remain);

  size_t chksum_data = Serialization::decode_i32(&ptr, &remain);
      
  if(!checksum_i32_chk(Serialization::decode_i32(&ptr, &remain), 
                        read_buf.base, HEADER_SIZE, HEADER_OFFSET_CHKSUM) || 
     !checksum_i32_chk(chksum_data, ptr, sz)) {
    err = Error::CHECKSUM_MISMATCH;
  } else {
    schema = std::make_shared<DB::Schema>(&ptr, &sz);
  }
}

DB::Schema::Ptr load(int &err, int64_t cid, 
                     uint8_t replication, bool recover=true) {

  DB::Schema::Ptr schema = nullptr;
  try{
    load(err, filepath(cid), schema);
  } catch (const std::exception& e) {
    SWC_LOGF(LOG_ERROR, "schema load exception (%s)", e.what());
    schema = nullptr;
  }

  if(schema == nullptr && err != Error::SERVER_SHUTTING_DOWN && recover){
    SWC_LOGF(LOG_WARN, "Missing Column(cid=%d) Schema", cid);

    schema = DB::Schema::make();
    schema->cid = cid;

    if(cid <= SYS_CID_END) {
      err == Error::OK;
      schema->col_name.append("SYS_");
      if(cid == 9) {
        schema->col_name.append("STATS");
        schema->col_type = Types::Column::COUNTER_I64;
        schema->col_seq = Types::KeySeq::LEXIC;
        schema->cell_ttl = Env::Config::settings()->get_i32(
          "swc.stats.ttl", 1036800);
      } else {
        schema->col_seq = Types::MetaColumn::get_seq_type(cid);
        schema->col_name.append(
          (Types::MetaColumn::is_master(cid) ? "MASTER_": "META_") 
          + Types::to_string(schema->col_seq) );
      }
    
    } else {
     // schema backups || instant create || throw ?
      schema->col_name.append("noname_");
      schema->col_name.append(std::to_string(cid));
    }

    SWC_LOGF(LOG_WARN, "Missing Column(cid=%d) Schema set to %s", 
              cid, schema->to_string().c_str());
    save(err, schema, replication);
  }
  
  return schema;
}


void save_with_validation(int &err, DB::Schema::Ptr schema_save,
                          uint8_t replication) {
  save(err, schema_save, replication); // ?tmp-file 
  if(err != Error::OK) 
    return;

  DB::Schema::Ptr schema_new = load(err, schema_save->cid, replication, false);
  if(err != Error::OK) 
    return;

  if(!schema_new->equal(schema_save)) {
    err = Error::COLUMN_SCHEMA_BAD_SAVE;
    return;
  }
}

}}}

#endif // swc_manager_db_Schema_h