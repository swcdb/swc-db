/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 */

#ifndef swc_fs_Broker_Protocol_params_Seek_h
#define swc_fs_Broker_Protocol_params_Seek_h



namespace SWC { namespace FS { namespace Protocol { namespace Params {


class SeekReq : public Serializable {
  public:

  SeekReq() {}

  SeekReq(int32_t fd, size_t offset)
          : fd(fd), offset(offset) {}

  int32_t   fd;
  uint64_t  offset;

  private:

  size_t encoded_length_internal() const {
    return 12;
  }

  void encode_internal(uint8_t** bufp) const {
    Serialization::encode_i32(bufp, fd);
    Serialization::encode_i64(bufp, offset);
  }

  void decode_internal(const uint8_t** bufp, size_t* remainp) {
    fd = (int32_t)Serialization::decode_i32(bufp, remainp);
    offset = Serialization::decode_i64(bufp, remainp);
  }
  
};




class SeekRsp : public Serializable {
  public:

  SeekRsp() {}

  SeekRsp(size_t offset) : offset(offset) {}
  
  uint64_t offset;
  
  private:
  
  size_t encoded_length_internal() const {
    return 8;
  }

  void encode_internal(uint8_t** bufp) const {
    Serialization::encode_i64(bufp, offset);
  }

  void decode_internal(const uint8_t** bufp, size_t* remainp) {
    offset = Serialization::decode_i64(bufp, remainp);
  }
  
};

}}}}

#endif // swc_fs_Broker_Protocol_params_Seek_h
