/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */


#ifndef swcdb_db_cells_CellKey_h
#define swcdb_db_cells_CellKey_h

#include <cassert>
#include <vector>
#include "swcdb/core/Serialization.h"
#include "swcdb/core/Comparators.h"
#include "swcdb/db/Cells/CellKeyVec.h"

namespace SWC { namespace DB { namespace Cell {

class Key final {
  public:

  typedef std::shared_ptr<Key> Ptr;

  explicit Key(bool own = true): own(own), count(0), size(0), data(0) { }

  explicit Key(const Key &other)
              : own(other.size), count(other.count), size(other.size),
                data(_data(other.data)) {
  }

  void copy(const Key &other) {
    free(); 
    own = true;
    size = other.size;
    count = other.count;
    data = _data(other.data);
  }

  ~Key() {
    if(own && data)
      delete [] data;
  }

  void free() {
    if(own && data)
      delete [] data;
    data = 0;
    size = 0;
    count = 0;
  }
  
  const bool sane() const {
    return (count && size && data) || (!count && !size && !data);
  }
  
  void add(const std::string_view& fraction) {
    add((const uint8_t*)fraction.data(), fraction.length());
  }

  void add(const std::string& fraction) {
    add((const uint8_t*)fraction.data(), fraction.length());
  }

  void add(const char* fraction) {
    add((const uint8_t*)fraction, strlen(fraction));
  }

  void add(const char* fraction, uint32_t len) {
    add((const uint8_t*)fraction, len);
  }

  void add(const uint8_t* fraction, uint32_t len) {
    const uint8_t* old = data;
    uint32_t old_size = size;

    uint8_t* ptr = data = 
      new uint8_t[size += Serialization::encoded_length_vi32(len) + len];
    if(old) {
      memcpy(ptr, old, old_size);
      ptr += old_size;
      if(own)
        delete [] old;
    }
    Serialization::encode_vi32(&ptr, len);
    memcpy(ptr, fraction, len);
    ++count;
    own = true;
  }

  void insert(uint32_t idx, const std::string& fraction) {
    insert(idx, (const uint8_t*)fraction.data(), fraction.length());
  }

  void insert(uint32_t idx, const char* fraction) {
    insert(idx, (const uint8_t*)fraction, strlen(fraction));
  }

  void insert(uint32_t idx, const char* fraction, uint32_t len) {
    insert(idx, (const uint8_t*)fraction, len);
  }

  void insert(uint32_t idx, const uint8_t* fraction, uint32_t len) {
    if(!data || idx >= count) {
      add(fraction, len);
      return;
    }

    uint32_t prev_size = size;
    uint32_t f_size = Serialization::encoded_length_vi32(len) + len;
    size += f_size;

    uint8_t* data_tmp = new uint8_t[size]; 
    const uint8_t* ptr_tmp = data;
   
    uint8_t* fraction_ptr;
    uint32_t pos = 0;
    uint32_t offset = 0;

    for(;;) {
      if(idx == pos++) {
        if(offset) 
          memcpy(data_tmp, data, offset);
        fraction_ptr = data_tmp + offset;
        Serialization::encode_vi32(&fraction_ptr, len);
        memcpy(fraction_ptr, fraction, len);
        fraction_ptr += len;
        break;
      }
      ptr_tmp += Serialization::decode_vi32(&ptr_tmp);
      offset += ptr_tmp-data;
    }
    
    if(prev_size-offset)
      memcpy(fraction_ptr, ptr_tmp, prev_size-offset);
    
    if(own)
      delete [] data;
    else
      own = true;
    data = data_tmp;
    ++count;
  }
  
  void remove(uint32_t idx, bool recursive=false) {
    if(!data || idx >= count)
      return;

    const uint8_t* ptr_tmp = data;
    if(!own) {
      own = true;
      ptr_tmp = (data = _data(data));
    }

    uint8_t* begin;
    for(uint32_t offset = 0; offset < count; ++offset) {
      begin = (uint8_t*)ptr_tmp;
      ptr_tmp += Serialization::decode_vi32(&ptr_tmp);
      if(offset < idx) 
        continue;
      
      if(recursive) {
        count = offset;
        size = begin-data;
      } else if(--count) {
        memmove(begin, ptr_tmp, size-(ptr_tmp-data)); 
        size -= ptr_tmp-begin;
      }
      
      ptr_tmp = data;
      if(count) {
        data = _data(ptr_tmp);
      } else {
        data = 0;
        size = 0;
      }
      delete ptr_tmp;
      break;
    }
  }

  const std::string get_string(uint32_t idx) const {
    const char* fraction;
    uint32_t length;
    get(idx, &fraction, &length);
    return std::string(fraction, length);
  }

  void get(uint32_t idx, const char** fraction, uint32_t* length) const {
    if(data && ++idx <= count) {
      const uint8_t* ptr = data;
      for(; idx ; --idx)
        ptr += (*length = Serialization::decode_vi32(&ptr));
      if(!idx) { 
        *fraction = (const char*)ptr - *length;
        return;
      }
    }
    *fraction = 0;
    *length = 0;
  }

  const bool equal(const Key &other) const {
    return count == other.count && 
          ((!data && !other.data) || 
           Condition::eq(data, size, other.data, other.size));
  }

  const Condition::Comp compare(const Key &other, uint32_t fractions=0, 
                                bool empty_ok=false) const {
    Condition::Comp comp = Condition::EQ;
    const uint8_t* ptr = data;
    uint32_t len;
    const uint8_t* ptr_other = other.data;
    uint32_t len_other;
    
    uint32_t max = fractions ? fractions 
                  : (count > other.count ? count : other.count);
    for(uint32_t c = 0; c<max; ++c, ptr += len, ptr_other += len_other) {

      if(c == count || c == other.count)
        return count > other.count ? Condition::LT : Condition::GT;

      len_other = Serialization::decode_vi32(&ptr_other);
      if(!(len = Serialization::decode_vi32(&ptr)) && empty_ok)
        continue;
      if((comp = Condition::condition(ptr, len, ptr_other, len_other)) 
                  != Condition::EQ)
        return comp;
    }
    return comp;
  }

  const bool align(KeyVec& start, KeyVec& finish) const {
    const uint8_t* ptr = data;
    uint32_t len;
    bool chg = false;
    for(uint32_t c = 0; c < count; ++c) {
      len = Serialization::decode_vi32(&ptr);

      if(c == start.size()) {
        start.add(ptr, len);
        chg = true;
      } else if(Condition::condition(
                  (const uint8_t*)start[c].data(), start[c].length(),
                  ptr, len
              ) == Condition::LT) {
        start.set(c, ptr, len);
        chg = true;
      }

      if(c == finish.size()) {
        finish.add(ptr, len);
        chg = true;
      } else if(Condition::condition(
                  (const uint8_t*)finish[c].data(), finish[c].length(),
                  ptr, len
                ) == Condition::GT) {
        finish.set(c, ptr, len);
        chg = true;
      }
      ptr += len;
    }
    return chg;
  }

  const bool compare(const KeyVec& other, Condition::Comp break_if,
                     uint32_t max = 0, bool empty_ok=false) const {
    const uint8_t* ptr = data;
    uint32_t len = 0;
    if(!max)
      max = count > other.size() ? count : other.size();
    for(uint32_t c = 0; c<max; ++c, ptr += len) {

      if(c == count || c == other.size())
        return count > other.size() 
              ? break_if != Condition::LT 
              : break_if != Condition::GT;

      if(!(len = Serialization::decode_vi32(&ptr)) && empty_ok)
        continue;

      auto& r = other[c];
      if(Condition::condition(ptr, len, (const uint8_t*)r.data(), r.length())
                                                              == break_if)
        return false;
    }
    return true;
  }

  const bool empty() const {
    return !count;
  }
  
  const uint32_t encoded_length() const {
    return Serialization::encoded_length_vi32(count) + size;;
  }
  
  void encode(uint8_t **bufp) const {
    Serialization::encode_vi32(bufp, count);
    if(size) {
      memcpy(*bufp, data, size);
      *bufp += size;
    }
  }

  void decode(const uint8_t **bufp, size_t* remainp, bool owner=false) {
    free();
    own = owner;
    if(count = Serialization::decode_vi32(bufp, remainp)) {
      const uint8_t* ptr_start = *bufp;
      for(uint32_t n=0; n<count; ++n) 
        *bufp += Serialization::decode_vi32(bufp);
      size = *bufp - ptr_start;
      *remainp -= size;
      data = own ? _data(ptr_start) : (uint8_t*)ptr_start;
    }
  }

  void convert_to(std::vector<std::string>& key) const {
    uint32_t len = 0;
    const uint8_t* ptr = data;
    key.clear();
    key.resize(count);
    for(auto it = key.begin(); it<key.end(); ++it, ptr+=len) {
      len = Serialization::decode_vi32(&ptr);
      it->append((const char*)ptr, len);
    }
  }

  void read(const std::vector<std::string>& key)  {
    free();
    for(auto& f : key)
      add(f);
  }

  const bool equal(const std::vector<std::string>& key) const {
    if(key.size() != count)
      return false;
      
    uint32_t len;
    const uint8_t* ptr = data;
    for(auto it = key.begin(); it<key.end(); ++it, ptr+=len) {
      len = Serialization::decode_vi32(&ptr);
      if(!Condition::eq(ptr, len, (const uint8_t*)it->data(), it->length()))
        return false;
    }
    return true;
  }

  const std::string to_string() const {
    std::string s("Key(");
    s.append("sz=");
    s.append(std::to_string(count));
    s.append(" len=");
    s.append(std::to_string(size));
    s.append(" fractions=[");
    uint32_t len = 0;
    const uint8_t* ptr = data;
    for(uint32_t n=0; n<count; ++n,ptr+=len) {
      s.append("(");
      len = Serialization::decode_vi32(&ptr);
      s.append((const char*)ptr, len);
      s.append("),");
    }
    s.append("])");
    return s;
  }
  
  void display(std::ostream& out, bool pretty=false) const {
    out << "["; 
    if(!count) {
      out << "]"; 
      return;
    }
      
    uint32_t len;
    const uint8_t* ptr = data;
    char hex[2];
    for(uint32_t n=0; n<count; ) {
      for(len = Serialization::decode_vi32(&ptr); len--; ++ptr) {
        if(pretty && (*ptr < 32 || *ptr > 126)) {
          sprintf(hex, "%X", *ptr);
          out << "0x" << hex;
        } else
          out << *ptr; 
      }
      if(++n < count)
        out << ", "; 
    }
    out << "]"; 
    
  }

  void display_details(std::ostream& out, bool pretty=false) const {
    out << "size=" << size << " count=" << count << " fractions=[";
    uint32_t len;
    const uint8_t* ptr = data;
    char hex[2];
    for(uint32_t n=0; n<count; ) {
      out << '"';
      for(len = Serialization::decode_vi32(&ptr); len--; ++ptr) {
        if(pretty && (*ptr < 32 || *ptr > 126)) {
          sprintf(hex, "%X", *ptr);
          out << "0x" << hex;
        } else
          out << *ptr; 
      }
      out << '"';
      if(++n < count)
        out << ", "; 
    }
    out << "]"; 
  }

  bool      own;
  uint32_t  count;
  uint32_t  size;
  uint8_t*  data;

  private:
  
  uint8_t* _data(const uint8_t* ptr) {
    return size ? (uint8_t*)memcpy(new uint8_t[size], ptr, size) : 0;
  }
  
};

}}}

#endif