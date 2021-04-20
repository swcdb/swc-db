/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */

#ifndef swcdb_fs_Broker_Protocol_params_Flush_h
#define swcdb_fs_Broker_Protocol_params_Flush_h


#include "swcdb/core/comm/Serializable.h"


namespace SWC { namespace Comm { namespace Protocol {
namespace FsBroker {  namespace Params {


class FlushReq final : public Serializable {
  public:

  FlushReq();

  FlushReq(int32_t fd);

  int32_t fd;

  private:

  size_t internal_encoded_length() const override;

  void internal_encode(uint8_t** bufp) const override;

  void internal_decode(const uint8_t** bufp, size_t* remainp) override;

};

}}}}}


#if defined(SWC_IMPL_SOURCE) or \
    (defined(FS_BROKER_APP) and !defined(BUILTIN_FS_BROKER))
#include "swcdb/fs/Broker/Protocol/params/Flush.cc"
#endif


#endif // swcdb_fs_Broker_Protocol_params_Flush_h
