/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */


#ifndef swc_app_ranger_handlers_AssignId_h
#define swc_app_ranger_handlers_AssignId_h


namespace SWC { namespace Protocol { namespace Rgr { namespace Handler {


class AssignId : public AppHandler {
  public:

  AssignId(ConnHandlerPtr conn, Event::Ptr ev, 
          Mngr::Req::RgrMngId::Scheduler* validator)
          : AppHandler(conn, ev), validator(validator) { }

  void run() override {

    try {
      if(Env::RgrData::is_shuttingdown()){
        m_conn->send_error(Error::SERVER_SHUTTING_DOWN, "", m_ev);
        return;
      }
      m_conn->response_ok(m_ev);
      Mngr::Req::RgrMngId::assign(validator);
    }
    catch (Exception &e) {
      SWC_LOG_OUT(LOG_ERROR) << e << SWC_LOG_OUT_END;
    }
  
  }

  private:
  Mngr::Req::RgrMngId::Scheduler* validator;
};
  

}}}}

#endif // swc_app_ranger_handlers_AssignId_h