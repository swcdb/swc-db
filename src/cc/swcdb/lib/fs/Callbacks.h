/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */

#ifndef swc_lib_fs_Callbacks_h
#define swc_lib_fs_Callbacks_h

#include <functional>
#include "swcdb/lib/core/StaticBuffer.h"

namespace SWC{ namespace FS {

namespace Callback {

  typedef std::function<void(int, bool)>         ExistsCb_t;
  typedef std::function<void(int)>               RemoveCb_t;
  typedef std::function<void(int, size_t)>       LengthCb_t;

  typedef std::function<void(int)>               MkdirsCb_t;
  typedef std::function<void(int, DirentList)>   ReaddirCb_t;
  typedef std::function<void(int)>               RmdirCb_t;
  typedef std::function<void(int)>               RenameCb_t;
  

  typedef std::function<void(int, SmartFdPtr)>                  WriteCb_t;
  typedef std::function<void(int, SmartFdPtr)>                  CreateCb_t;
  typedef std::function<void(int, SmartFdPtr)>                  OpenCb_t;
  typedef std::function<void(int, SmartFdPtr, StaticBufferPtr)> ReadCb_t;
  typedef std::function<void(int, SmartFdPtr, StaticBufferPtr)> PreadCb_t;
  typedef std::function<void(int, SmartFdPtr, size_t)>          AppendCb_t;
  typedef std::function<void(int, SmartFdPtr)>                  SeekCb_t;
  typedef std::function<void(int, SmartFdPtr)>                  FlushCb_t;
  typedef std::function<void(int, SmartFdPtr)>                  SyncCb_t;
  typedef std::function<void(int, SmartFdPtr)>                  CloseCb_t;
 
}

}}

#endif  // swc_lib_fs_Callbacks_h