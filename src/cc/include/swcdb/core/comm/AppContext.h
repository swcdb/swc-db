
/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */

#ifndef swcdb_core_comm_AppContext_h
#define swcdb_core_comm_AppContext_h


#include "swcdb/core/comm/Event.h"
#include "swcdb/core/comm/Resolver.h"


namespace SWC { namespace Comm {

// forward declarations
class ConnHandler;
typedef std::shared_ptr<ConnHandler> ConnHandlerPtr;


class AppContext : public std::enable_shared_from_this<AppContext> {
  public:

  typedef std::shared_ptr<AppContext> Ptr;

  AppContext(Config::Property::V_GENUM::Ptr cfg_encoder) noexcept;

  virtual ~AppContext() { };

  virtual void handle(ConnHandlerPtr conn, const Event::Ptr& ev) = 0;

  virtual void init(const Comm::EndPoints&) { }

  virtual void stop();

  const Config::Property::V_GENUM::Ptr cfg_encoder;

};


}} // namespace SWC::Comm



#ifdef SWC_IMPL_SOURCE
#include "swcdb/core/comm/AppContext.cc"
#endif

#endif // swcdb_core_comm_AppContext_h