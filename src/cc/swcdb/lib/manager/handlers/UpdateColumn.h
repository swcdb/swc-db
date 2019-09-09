/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */


#ifndef swc_app_manager_handlers_UpdateColumn_h
#define swc_app_manager_handlers_UpdateColumn_h

#include "swcdb/lib/db/Protocol/params/MngrUpdateColumn.h"


namespace SWC { namespace server { namespace Mngr {

namespace Handler {


class UpdateColumn : public AppHandler {
  public:

    UpdateColumn(ConnHandlerPtr conn, EventPtr ev)
                : AppHandler(conn, ev){}

  void run() override {
    try {

      const uint8_t *ptr = m_ev->payload;
      size_t remain = m_ev->payload_len;

      Protocol::Params::MngrUpdateColumn params;
      params.decode(&ptr, &remain);
      
      m_conn->response_ok(m_ev);
      
      Env::RangeServers::get()->update_status(
        params.function, params.schema, params.err);

    } catch (Exception &e) {
      HT_ERROR_OUT << e << HT_END;
    }
  }

};
  

}}}}

#endif // swc_app_manager_handlers_UpdateColumn_h