/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */


#ifndef swcdb_core_String_h
#define swcdb_core_String_h


#include <string>
#include <sstream>


namespace SWC {


/*!
 *  \addtogroup Core
 *  @{
 */


  std::string format(const char *fmt, ...)
              __attribute__((format(printf, 1, 2)));


  template <class SequenceT>
  std::string format_list(const SequenceT &seq, const char *sep = ", ") {
    if(seq.empty())
      return "[]";

    auto it = seq.begin();
    auto end = seq.end();

    std::ostringstream out;
    out << '[';
    more:
      out << *it;
      if(++it != end) {
        out << sep;
        goto more;
      }
    out << ']';

    return out.str();
  }


/*! @} End of Core Group*/


}

#ifdef SWC_IMPL_SOURCE
#include "swcdb/core/String.cc"
#endif 

#endif // swcdb_core_String_h
