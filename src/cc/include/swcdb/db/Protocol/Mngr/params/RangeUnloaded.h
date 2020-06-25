
/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 */

#ifndef swc_db_protocol_mngr_params_RangeUnloaded_h
#define swc_db_protocol_mngr_params_RangeUnloaded_h

#include "swcdb/core/Serializable.h"


namespace SWC { namespace Protocol { namespace Mngr { namespace Params {


class RangeUnloadedReq : public Serializable {
  public:

  RangeUnloadedReq(cid_t cid=0, rid_t rid=0) 
                  : cid(cid), rid(rid) {
  }

  virtual ~RangeUnloadedReq(){ }
  
  cid_t        cid;
  rid_t        rid;
  
  std::string to_string() const {
    std::string s("RangeUnloadedReq(");
    s.append("cid=");
    s.append(std::to_string(cid));
    s.append(" rid=");
    s.append(std::to_string(rid));
    return s;
  }

  private:

  size_t encoded_length_internal() const {
    return Serialization::encoded_length_vi64(cid)
      + Serialization::encoded_length_vi64(rid);
  }
    
  void encode_internal(uint8_t** bufp) const {
    Serialization::encode_vi64(bufp, cid);
    Serialization::encode_vi64(bufp, rid);
  }
    
  void decode_internal(const uint8_t** bufp, size_t* remainp) {
    cid = Serialization::decode_vi64(bufp, remainp);
    rid = Serialization::decode_vi64(bufp, remainp);
  }

};



class RangeUnloadedRsp : public Serializable {
  public:

  RangeUnloadedRsp(): err(0) {}

  virtual ~RangeUnloadedRsp() {}
  
  int  err;        

  std::string to_string() const {
    std::string s("RangeUnloadedRsp(");
    s.append("err=");
    s.append(std::to_string(err));
    if(err) {
      s.append("(");
      s.append(Error::get_text(err));
      s.append(")");
    }
    s.append(")");
    return s;
  }

  private:

  size_t encoded_length_internal() const {
    return Serialization::encoded_length_vi32(err);
  }
    
  void encode_internal(uint8_t** bufp) const {
    Serialization::encode_vi32(bufp, err);
  }
    
  void decode_internal(const uint8_t** bufp, size_t* remainp) {
    err = Serialization::decode_vi32(bufp, remainp);
  }

};
  

}}}}

#endif // swc_db_protocol_mngr_params_RangeUnloaded_h
