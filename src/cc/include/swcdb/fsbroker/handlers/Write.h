/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */


#ifndef swcdb_fsbroker_handlers_Write_h
#define swcdb_fsbroker_handlers_Write_h

#include "swcdb/fs/Broker/Protocol/params/Write.h"

namespace SWC { namespace Comm { namespace Protocol {
namespace FsBroker {  namespace Handler {



void write(const ConnHandlerPtr& conn, const Event::Ptr& ev) {

  int err = Error::OK;
  try {

    const uint8_t *ptr = ev->data.base;
    size_t remain = ev->data.size;

    Params::WriteReq params;
    params.decode(&ptr, &remain);

    auto smartfd = FS::SmartFd::make_ptr(params.fname, params.flags);
    auto fs = Env::FsInterface::fs();

    //Env::FsInterface::fs()->write(
    //  err, smartfd, params.replication, params.blksz, ev->data_ext
    //); needs fds state

    fs->create(err, smartfd, 0, params.replication, params.blksz);
    if(smartfd->valid()) {
      int32_t fd = Env::FsBroker::fds()->add(smartfd);
      
      if(!err && ev->data_ext.size)
        fs->append(err, smartfd, ev->data_ext, FS::Flags::FLUSH);
      
      if((smartfd = Env::FsBroker::fds()->remove(fd))) {
        int errtmp;
        fs->close(!err ? err : errtmp, smartfd);
      }
    } else if(!err) {
      err = Error::FS_BAD_FILE_HANDLE;
    }

  } catch(...) {
    const Error::Exception& e = SWC_CURRENT_EXCEPTION("");
    SWC_LOG_OUT(LOG_ERROR, SWC_LOG_OSTREAM << e; );
    err = e.code();
  }
  
  if(ev->expired())
    return;

  try {
    auto cbp = Buffers::make(4);
    cbp->header.initialize_from_request_header(ev->header);
    cbp->append_i32(err);
    conn->send_response(cbp);

  } catch(...) {
    SWC_LOG_CURRENT_EXCEPTION("");
  }

}
  

}}}}}

#endif // swcdb_fsbroker_handlers_Write_h
