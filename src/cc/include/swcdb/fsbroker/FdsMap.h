/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 */

#ifndef swc_fsbroker_FdsMap_h
#define swc_fsbroker_FdsMap_h

#include <unordered_map>
#include <shared_mutex>

namespace SWC { namespace FsBroker {

class Fds final {

  public:

  typedef Fds* Ptr;

  typedef std::unordered_map<int32_t, FS::SmartFd::Ptr> FdsMap;
  typedef std::pair<int32_t, FS::SmartFd::Ptr>          FdpPair;
  
  Fds() : m_next_fd(0) {}
  
  ~Fds(){}

  int32_t add(const FS::SmartFd::Ptr& fd) {
    for(;;) {
      Mutex::scope lock(m_mutex);
      if(m_fds.emplace(++m_next_fd < 0 ? m_next_fd=1 : m_next_fd, fd).second)
        break;
    }
    return m_next_fd;
  }

  FS::SmartFd::Ptr remove(int32_t fd) {
    Mutex::scope lock(m_mutex);
    
    auto it = m_fds.find(fd);
    if(it == m_fds.end())
      return nullptr;
    FS::SmartFd::Ptr smartfd = std::move(it->second);
    m_fds.erase(it);
    return smartfd;
  }

  FS::SmartFd::Ptr select(int32_t fd) {
    Mutex::scope lock(m_mutex);
    
    auto it = m_fds.find(fd);
    if(it != m_fds.end())
      return it->second;
    return nullptr;
  }

  FS::SmartFd::Ptr pop_next() {
    Mutex::scope lock(m_mutex);
    
    auto it = m_fds.begin();
    if(it == m_fds.end())
      return nullptr;
    FS::SmartFd::Ptr smartfd = std::move(it->second);
    m_fds.erase(it);
    return smartfd;
  }

  private:
  Mutex               m_mutex;
  int32_t             m_next_fd;
  std::unordered_map<int32_t, FS::SmartFd::Ptr> m_fds;
};

} // namespace FsBroker


namespace Env {
class Fds final {
  public:

  static void init() {
    m_env = std::make_shared<Fds>();
  }

  static FsBroker::Fds::Ptr get(){
    SWC_ASSERT(m_env != nullptr);
    return m_env->m_fds;
  }

  Fds() : m_fds(new FsBroker::Fds()) {}

  ~Fds(){
    if(m_fds != nullptr)  
      delete m_fds;
  }

  private:
  FsBroker::Fds::Ptr         m_fds = nullptr;
  inline static std::shared_ptr<Fds> m_env = nullptr;
};


}}

#endif // swc_fsbroker_FdsMap_h