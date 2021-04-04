/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */


#ifndef swcdb_db_cells_SpecsTimestamp_h
#define swcdb_db_cells_SpecsTimestamp_h


#include "swcdb/core/Comparators.h"


namespace SWC { namespace DB { namespace Specs {

     
class Timestamp {
  public:

  explicit Timestamp() noexcept;

  explicit Timestamp(int64_t timestamp, Condition::Comp comp) noexcept;

  void copy(const Timestamp &other) noexcept;

  void set(int64_t timestamp, Condition::Comp comperator) noexcept;

  void free() noexcept;

  ~Timestamp();

  bool empty() const noexcept;

  bool equal(const Timestamp &other) const noexcept;

  size_t encoded_length() const noexcept;

  void encode(uint8_t** bufp) const;

  void decode(const uint8_t** bufp, size_t* remainp);

  bool is_matching(int64_t other) const noexcept;

  std::string to_string() const;

  void display(std::ostream& out) const;

  void print(std::ostream& out) const;

  friend std::ostream& operator<<(std::ostream& out, const Timestamp& key) {
    key.print(out);
    return out;
  }

  int64_t          value;
  Condition::Comp  comp;
  bool             was_set;

};


}}}

#ifdef SWC_IMPL_SOURCE
#include "swcdb/db/Cells/SpecsTimestamp.cc"
#endif

#endif // swcdb_db_cells_SpecsTimestamp_h
