/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */


#ifndef swcdb_manager_Protocol_handlers_ColumnUpdate_h
#define swcdb_manager_Protocol_handlers_ColumnUpdate_h

#include "swcdb/manager/Protocol/Mngr/params/ColumnUpdate.h"


namespace SWC { namespace Comm { namespace Protocol {
namespace Mngr { namespace Handler {


void column_update(const ConnHandlerPtr& conn, const Event::Ptr& ev) {
  try {
    const uint8_t *ptr = ev->data.base;
    size_t remain = ev->data.size;

    Params::ColumnUpdate params;
    params.decode(&ptr, &remain);
      
    conn->response_ok(ev);
      
    Env::Mngr::mngd_columns()->update_status(
      params.function, params.schema, params.err);

  } catch(...) {
    SWC_LOG_CURRENT_EXCEPTION("");
  }
}

  

}}}}}

#endif // swcdb_manager_Protocol_handlers_ColumnUpdate_h
