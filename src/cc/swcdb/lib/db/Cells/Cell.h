/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */


#ifndef swcdb_db_cells_Cell_h
#define swcdb_db_cells_Cell_h

#include <cassert>
#include "swcdb/lib/core/Time.h"
#include "swcdb/lib/core/DynamicBuffer.h"

#include "swcdb/lib/db/Types/Column.h"
#include "CellKey.h"


namespace SWC { namespace DB { namespace Cells {

enum Flag {
  INSERT                    = 0x0,
  DELETE                    = 0x1,
  DELETE_VERSION            = 0x2,
  DELETE_FRACTION           = 0x3,
  DELETE_FRACTION_VERSION   = 0x4
};

static const int64_t TIMESTAMP_MIN  = INT64_MIN;
static const int64_t TIMESTAMP_MAX  = INT64_MAX;
static const int64_t TIMESTAMP_NULL = INT64_MIN + 1;
static const int64_t TIMESTAMP_AUTO = INT64_MIN + 2;
static const int64_t AUTO_ASSIGN    = TIMESTAMP_AUTO;

static const uint8_t HAVE_REVISION      =  0x80;
static const uint8_t HAVE_TIMESTAMP     =  0x40;
static const uint8_t AUTO_TIMESTAMP     =  0x20;
static const uint8_t REV_IS_TS          =  0x10;
static const uint8_t HAVE_ON_FRACTION   =  0x8;
//static const uint8_t TYPE_DEFINED     =  0x2;
static const uint8_t TS_DESC            =  0x1;

enum OP {
  EQUAL  = 0x0,
  PLUS   = 0x1,
  MINUS  = 0x2,
};


static inline void get_key_fwd_to_cell_end(DB::Cell::Key& key, 
                                           uint8_t** base, size_t* remainp){
  const uint8_t* ptr = *base;
  ptr++;
  *remainp-=1;
  key.decode(&ptr, remainp);
  uint8_t control = *ptr++;
  *remainp-=1;
  if(control & HAVE_ON_FRACTION)
    Serialization::decode_vi32(&ptr, remainp);

  if(control & HAVE_TIMESTAMP || control & AUTO_TIMESTAMP) 
    Serialization::decode_vi64(&ptr, remainp);

  if(control & HAVE_REVISION) 
    Serialization::decode_vi64(&ptr, remainp);

  uint32_t vlen = Serialization::decode_vi32(&ptr, remainp);
  ptr += vlen;
  *remainp -= vlen;
  *base = (uint8_t*)ptr;
}

  
class Cell {
  public:
  typedef std::shared_ptr<Cell> Ptr;

  static bool is_next(uint8_t flag, SWC::DynamicBuffer &src_buf) {
    return *src_buf.ptr == flag;
  }


  explicit Cell():  flag(0), control(0), 
                    on_fraction(0), timestamp(0), revision(0),
                    value(0), vlen(0), own(false) { }

  explicit Cell(const Cell& other){
    copy(other);
  }

  void copy(const Cell& other) {
    own = true;
    //std::cout << " copy(const Cell &other) vlen=" << other.vlen << "\n";
    flag        = other.flag;
    key.copy(other.key);
    control     = other.control;
    on_fraction = other.on_fraction;
    timestamp   = other.timestamp;
    revision    = other.revision;
    vlen        = other.vlen;
    if(vlen > 0) {
      value = new uint8_t[vlen];
      memcpy(value, other.value, vlen);
    }
  }

  virtual ~Cell(){
    //std::cout << " ~Cell\n";
    if(own && value != 0) 
      delete [] value;
    key.free();
  }

  inline void free(){
    if(own && value != 0) {
      delete [] value;
      vlen = 0;
      value = 0;
    }
  }

  void set_flag(uint8_t nflag, uint32_t fraction = 0){
    flag = nflag;
    if(fraction != 0 
        && (flag == INSERT 
         || flag == DELETE_FRACTION 
         || flag == DELETE_FRACTION_VERSION)) {
      control |= HAVE_ON_FRACTION;
      on_fraction = fraction;
    }
  }

  void set_time_order_desc(bool desc){
    if(desc)  control |= TS_DESC;
    else      control != TS_DESC;
  }

  void set_timestamp(int64_t ts){
    timestamp = ts;
    control |= HAVE_TIMESTAMP;
  }

  void set_revision(uint64_t rev){
    revision = rev;
    control |= HAVE_REVISION;
  }

  // SET_VALUE
  void set_value(uint8_t* v, uint32_t len){
    value = v;
    vlen = len;
  }

  void set_value(const char* v, uint32_t len){
    set_value((uint8_t *)v, len);
  }

  void set_value(const char* v){
    set_value((uint8_t *)v, strlen(v));
  }

  void set_value(OP op, int64_t v){
    free();
    if(v == 0 && op == OP::EQUAL) {
      vlen = 0;
      value = 0;
      return;
    }
    own = true;
    vlen = 1+Serialization::encoded_length_vi64(v);
    value = new uint8_t[vlen];
    uint8_t* ptr = value;
    *ptr++ = (uint8_t)op;
    // +? i64's storing epochs 
    Serialization::encode_vi64(&ptr, v);
  }

  int64_t get_value(OP *op) const {
    if(vlen == 0) {
      *op = OP::EQUAL;
      return vlen;
    }
    const uint8_t *ptr = value;
    *op = (OP)*ptr++;
    return Serialization::decode_vi64(&ptr);
  }
    
  // READ
  void read(uint8_t **base, size_t* remainp) {

    const uint8_t* ptr = *base;
    flag = *ptr++;
    *remainp -= 1;
    key.decode(&ptr, remainp);
    control = *ptr++;
    *remainp -= 1;

    if (control & HAVE_ON_FRACTION)
      on_fraction = Serialization::decode_vi32(&ptr, remainp);

    if (control & HAVE_TIMESTAMP){
      timestamp = Serialization::decode_i64(&ptr, remainp);
    } else if(control & AUTO_TIMESTAMP)
      timestamp = AUTO_ASSIGN;

    if (control & HAVE_REVISION) {
      revision = Serialization::decode_i64(&ptr, remainp);
    } else if (control & REV_IS_TS)
      revision = timestamp;
      
    free();
    // own = owner;
    vlen = Serialization::decode_vi32(&ptr, remainp);
    if(vlen > 0){
      if(own) {
        value = new uint8_t[vlen];
        memcpy(value, ptr, vlen);
      } else
        value = (uint8_t *)ptr;
      ptr += vlen;
      *remainp -= vlen;
    } else 
      value = 0;

    *base += ptr - *base;
  }

  // WRITE
  void write(SWC::DynamicBuffer &dst_buf){
    uint32_t klen = 1+key.encoded_length()+1;
    if(control & HAVE_ON_FRACTION)
      klen += Serialization::encoded_length_vi32(on_fraction);
    if(control & HAVE_TIMESTAMP)
      klen += 8;
    if(control & HAVE_REVISION)
      klen += 8;
    dst_buf.ensure(klen+vlen+Serialization::encoded_length_vi32(vlen));

    *dst_buf.ptr++ = flag;
    key.encode(&dst_buf.ptr);

    *dst_buf.ptr++ = control;
    if(control & HAVE_ON_FRACTION)
      Serialization::encode_vi32(&dst_buf.ptr, on_fraction);
    if(control & HAVE_TIMESTAMP)
      Serialization::encode_i64(&dst_buf.ptr, timestamp);
    if(control & HAVE_REVISION)
      Serialization::encode_i64(&dst_buf.ptr, revision);
      
    Serialization::encode_vi32(&dst_buf.ptr, vlen);
    if(vlen > 0)
      dst_buf.add_unchecked(value, vlen);

    assert(dst_buf.fill() <= dst_buf.size);
  }

  bool equal(Cell &other){
    return  flag == other.flag && 
            control == other.control &&
            on_fraction == other.on_fraction && 
            timestamp == other.timestamp && 
            revision == other.revision && 
            vlen == other.vlen &&
            key.equal(other.key) &&
            memcmp(value, other.value, vlen) == 0;
  }

  const bool removal() const {
    return flag > Flag::INSERT;
  }
  
  const bool is_removing(const int64_t rev) const {
    return removal() &&
      (revision >= rev 
        && (flag == DELETE || flag == DELETE_FRACTION ))
       ||
      (revision == rev 
        && (flag == DELETE_VERSION || flag == DELETE_FRACTION_VERSION ));
  }

  const bool has_expired(const uint64_t ttl) const {
    return ttl && Time::now_ns() >= timestamp + ttl;
  }

  const std::string to_string() const {
    std::string s("Cell(");
    s.append("flag=");
    s.append(std::to_string(flag));

    s.append(" key=");
    s.append(key.to_string());

    s.append(" control=");
    s.append(std::to_string(control));
    
    s.append(" on_fraction=");
    s.append(std::to_string(on_fraction));
    
    s.append(" ts=");
    s.append(std::to_string(timestamp));

    s.append(" rev=");
    s.append(std::to_string(revision));

    s.append(" value=(");     
    char c;
    for(int i=0; i<vlen;i++) {
      c = *(value+i);
      s.append(std::string(&c, 1));
    }
    s.append(", len=");
    s.append(std::to_string(vlen));
    s.append(")");
    return s;
  }

  uint8_t         flag;
  DB::Cell::Key   key;
  uint8_t         control;
  uint32_t        on_fraction;
  int64_t         timestamp;
  uint64_t        revision;
    
  uint8_t*        value;
  uint32_t        vlen;
  bool            own;
};

 
// (int ScanBlock::load()) ScanBlock::next(CellSerial)


}}}
#endif