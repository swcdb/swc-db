/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */


#include "swcdb/db/Protocol/Bkr/req/ColumnGet.h"
#include "swcdb/db/client/Clients.h"
#include "swcdb/db/Protocol/Commands.h"


namespace SWC { namespace Comm { namespace Protocol {
namespace Bkr { namespace Req {


SWC_SHOULD_INLINE
void ColumnGet::schema(const SWC::client::Clients::Ptr& clients,
                       const std::string& name, ColumnGet::Cb_t&& cb,
                       const uint32_t timeout) {
  request(clients, Flag::SCHEMA_BY_NAME, name, std::move(cb), timeout);
}

SWC_SHOULD_INLINE
void ColumnGet::schema(const SWC::client::Clients::Ptr& clients,
                       cid_t cid, ColumnGet::Cb_t&& cb,
                       const uint32_t timeout) {
  request(clients, Flag::SCHEMA_BY_ID, cid, std::move(cb), timeout);
}

SWC_SHOULD_INLINE
void ColumnGet::cid(const SWC::client::Clients::Ptr& clients,
                    const std::string& name, ColumnGet::Cb_t&& cb,
                    const uint32_t timeout) {
  request(clients, Flag::ID_BY_NAME, name, std::move(cb), timeout);
}

SWC_SHOULD_INLINE
void ColumnGet::request(const SWC::client::Clients::Ptr& clients,
                        ColumnGet::Flag flag, const std::string& name,
                        ColumnGet::Cb_t&& cb,
                        const uint32_t timeout) {
  std::make_shared<ColumnGet>(
    clients, Mngr::Params::ColumnGetReq(flag, name), std::move(cb), timeout
  )->run();
}

SWC_SHOULD_INLINE
void ColumnGet::request(const SWC::client::Clients::Ptr& clients,
                        ColumnGet::Flag flag, cid_t cid,
                        ColumnGet::Cb_t&& cb, const uint32_t timeout) {
  std::make_shared<ColumnGet>(
    clients, Mngr::Params::ColumnGetReq(flag, cid), std::move(cb), timeout
  )->run();
}


ColumnGet::ColumnGet(const SWC::client::Clients::Ptr& clients,
                     const Mngr::Params::ColumnGetReq& params,
                     ColumnGet::Cb_t&& cb,
                     const uint32_t timeout)
                    : client::ConnQueue::ReqBase(
                        false,
                        Buffers::make(params, 0, COLUMN_GET, timeout)
                      ),
                      clients(clients), cb(std::move(cb)), bkr_idx(0) {
}

void ColumnGet::handle_no_conn() {
  if(clients->stopping()) {
    cb(req(), Error::CLIENT_STOPPING, Mngr::Params::ColumnGetRsp());
  } else {
    ++bkr_idx;
    run();
  }
}

bool ColumnGet::run() {
  EndPoints endpoints;
  while(!clients->stopping() &&
        (endpoints = clients->brokers.get_endpoints(bkr_idx)).empty()) {
    SWC_LOG(LOG_ERROR,
      "Broker hosts cfg 'swc.bkr.host' is empty, waiting!");
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
  if(endpoints.empty()) {
    handle_no_conn();
    return false;
  }
  clients->get_bkr_queue(endpoints)->put(req());
  return true;
}

void ColumnGet::handle(ConnHandlerPtr, const Event::Ptr& ev) {
  if(ev->type == Event::Type::DISCONNECT)
    return handle_no_conn();

  Mngr::Params::ColumnGetRsp rsp_params;
  int err = ev->response_code();
  if(!err) {
    try {
      const uint8_t *ptr = ev->data.base + 4;
      size_t remain = ev->data.size - 4;
      rsp_params.decode(&ptr, &remain);

    } catch(...) {
      const Error::Exception& e = SWC_CURRENT_EXCEPTION("");
      SWC_LOG_OUT(LOG_ERROR, SWC_LOG_OSTREAM << e; );
      err = e.code();
    }
  }

  cb(req(), err, rsp_params);
}



}}}}}
