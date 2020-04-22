/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 */


#include "swcdb/db/Cells/Mutable.h"


namespace SWC { namespace DB { namespace Cells { 


Mutable::Bucket* Mutable::make_bucket(uint16_t reserve) {
  auto bucket = new Bucket;
  if(reserve)
    bucket->reserve(reserve);
  return bucket;
}

Mutable::Ptr Mutable::make(const Types::KeySeq key_seq, 
                           const uint32_t max_revs, const uint64_t ttl_ns, 
                           const Types::Column type) {
  return std::make_shared<Mutable>(key_seq, max_revs, ttl_ns, type);
}

Mutable::Mutable(const Types::KeySeq key_seq,
                 const uint32_t max_revs, const uint64_t ttl_ns, 
                 const Types::Column type)
                : key_seq(key_seq), 
                  type(type), max_revs(max_revs), ttl(ttl_ns),
                  buckets({make_bucket(0)}), _bytes(0), _size(0) {
}

Mutable::Mutable(Mutable& other)
                : buckets(other.buckets), 
                  key_seq(other.key_seq),
                  _bytes(other.size_bytes()), _size(other.size()),
                  type(other.type), max_revs(other.max_revs), 
                  ttl(other.ttl) {
  other.buckets.clear();
  other.free();
}

void Mutable::take_sorted(Mutable& other) {
  if(!other.empty()) {
    if(empty()) {
      delete *buckets.begin();
      buckets.clear();
    }
    _bytes += other.size_bytes();
    _size += other.size();
    buckets.insert(buckets.end(), other.buckets.begin(), other.buckets.end());
    other.buckets.clear();
    other.free();
  }
}

Mutable::~Mutable() {
  free();
}

void Mutable::free() {
  for(auto bucket : buckets) {
    for(auto cell : *bucket)
      delete cell;
    delete bucket;
  }
  _bytes = 0;
  _size = 0;
  buckets.clear();
  buckets.push_back(make_bucket(0));
  buckets.shrink_to_fit();
}

void Mutable::reset(const uint32_t revs, const uint64_t ttl_ns, 
                    const Types::Column typ) {
  free();
  configure(revs, ttl_ns, typ);
}

void Mutable::configure(const uint32_t revs, const uint64_t ttl_ns, 
                        const Types::Column typ) {
  type = typ;
  max_revs = revs;
  ttl = ttl_ns;
}

Mutable::ConstIterator Mutable::ConstIt(size_t offset) const {
  return ConstIterator(&buckets, offset);
}

Mutable::Iterator Mutable::It(size_t offset) {
  return Iterator(&buckets, offset);
}

size_t Mutable::size() const {
  return _size;
}

size_t Mutable::size_bytes() const {
  return _bytes;
}

bool Mutable::empty() const {
  return !_size;
  //return buckets.empty() || buckets.front()->empty();
}

Cell*& Mutable::front() {
  return buckets.front()->front();
}

Cell*& Mutable::back() {
  return buckets.back()->back();
}

Cell*& Mutable::front() const {
  return buckets.front()->front();
}

Cell*& Mutable::back() const {
  return buckets.back()->back();
}

Cell*& Mutable::operator[](size_t idx) {
  return *Iterator(&buckets, idx).item;
}

bool Mutable::has_one_key() const {
  return  DB::KeySeq::compare(key_seq, front()->key, back()->key) 
            == Condition::EQ;
}


void Mutable::add_sorted(const Cell& cell, bool no_value) {
  Cell* adding;
  _add(adding = new Cell(cell, no_value));
  _push_back(adding);
}

void Mutable::add_sorted_no_cpy(Cell* cell) {
  _add(cell);
  _push_back(cell);
}

size_t Mutable::add_sorted(const uint8_t* ptr, size_t remain) {
  size_t count = 0;
  _bytes += remain;
  while(remain) {
    _push_back(new Cell(&ptr, &remain, true));
    ++count;
  }
  _size += count;
  return count;
}


void Mutable::add_raw(const DynamicBuffer& cells) {
  Cell cell;
  const uint8_t* ptr = cells.base;
  size_t remain = cells.fill();
  while(remain) {
    cell.read(&ptr, &remain);
    add_raw(cell);
  }
}

void Mutable::add_raw(const DynamicBuffer& cells, 
                      const DB::Cell::Key& upto_key,
                      const DB::Cell::Key& from_key) {
  Cell cell;
  const uint8_t* ptr = cells.base;
  size_t remain = cells.fill();
  while(remain) {
    cell.read(&ptr, &remain);
    if((!upto_key.empty() && 
       DB::KeySeq::compare(key_seq, upto_key, cell.key) != Condition::GT) ||
       DB::KeySeq::compare(key_seq, from_key, cell.key) == Condition::GT)
      add_raw(cell);
  }
}

void Mutable::add_raw(const Cell& e_cell) {
  size_t offset = _narrow(e_cell.key);

  if(e_cell.removal())
    _add_remove(e_cell, offset);

  else if(Types::is_counter(type))
    _add_counter(e_cell, offset);

  else
    _add_plain(e_cell, offset);
}


Cell* Mutable::takeout_begin(size_t idx) {
  return takeout(idx);
}

Cell* Mutable::takeout_end(size_t idx) {
  return takeout(size() - idx);
}

Cell* Mutable::takeout(size_t idx) {
  auto it = Iterator(&buckets, idx);
  Cell* cell;
  _remove(cell = *it.item);
  it.remove();
  return cell;
}


void Mutable::write_and_free(DynamicBuffer& cells, uint32_t& cell_count,
                             Interval& intval, uint32_t threshold, 
                             uint32_t max_cells) {
  if(!_size)
    return;
    
  cells.ensure(_bytes < threshold? _bytes: threshold);
  Cell* first = nullptr;
  Cell* last = nullptr;
  Cell* cell;
  size_t count = 0;
  auto it_start = Iterator(&buckets);
  for(auto it = it_start; it && (!threshold || threshold > cells.fill()) && 
                                (!max_cells || max_cells > cell_count); ++it) {
    ++count;
    if((cell=*it.item)->has_expired(ttl))
      continue;

    cell->write(cells);
    intval.expand(cell->timestamp);
    intval.align(cell->key);
    (first ? last : first) = cell;
    ++cell_count;
  }

  if(first) {
    intval.expand_begin(*first);
    intval.expand_end(*(last ? last : first));
  }
  
  if(_size == count)
    free();
  else
    _remove(it_start, count);
}

bool Mutable::write_and_free(const DB::Cell::Key& key_start, 
                             const DB::Cell::Key& key_finish,
                             DynamicBuffer& cells, uint32_t threshold) {
  bool more = _size;
  if(!more)
    return more;

  cells.ensure(_bytes < threshold? _bytes: threshold);

  auto it = Iterator(&buckets, _narrow(key_start));
  size_t count = 0;
  Iterator it_start;
  for(Cell* cell; it && (!threshold || threshold > cells.fill()); ++it) {
    cell=*it.item;

    if(!key_start.empty() && 
        DB::KeySeq::compare(key_seq, key_start, cell->key, 0) 
          == Condition::LT)
      continue;
    if(!key_finish.empty() && 
        DB::KeySeq::compare(key_seq, key_finish ,cell->key, 0) 
          == Condition::GT) {
      more = false;
      break;
    }

    if(!count)
      it_start = it;
    ++count;

    if(!cell->has_expired(ttl)) 
      cell->write(cells);
  }

  if(count) {
    if(count == _size) {
      free();
    } else {
      _remove(it_start, count);
      return more && it;
    }
  }
  return false;
}


std::string Mutable::to_string(bool with_cells) const {
  std::string s("Cells(size=");
  s.append(std::to_string(size()));
  s.append("/");
  s.append(std::to_string(_size));
  s.append(" bytes=");
  s.append(std::to_string(_bytes));
  s.append(" type=");
  s.append(Types::to_string(type));
  s.append(" max_revs=");
  s.append(std::to_string(max_revs));
  s.append(" ttl=");
  s.append(std::to_string(ttl));
  s.append(" buckets=");
  s.append(std::to_string(buckets.size()));
  
  if(with_cells) {
    size_t count = 0;
    s.append(" [\n");
    for(auto bucket : buckets) {
      s.append(" sz=");
      s.append(std::to_string(bucket->size()));
      s.append(" (\n");
      for(auto cell : *bucket) {
        s.append(cell->to_string(type));
        s.append("\n");
        ++count;
      }
     s.append(" )\n");
    }
    s.append("]");
    s.append(" counted=");
    s.append(std::to_string(count));
  }
  
  s.append(")");
  return s;
}


void Mutable::get(int32_t idx, DB::Cell::Key& key) const {
  key.copy((*ConstIterator(&buckets, idx < 0 ? size()+idx : idx).item)->key);
}
 
bool Mutable::get(const DB::Cell::Key& key, Condition::Comp comp, 
                  DB::Cell::Key& res) const {
  Condition::Comp chk;  
  for(auto it = ConstIterator(&buckets, _narrow(key)); it; ++it) {
    if((chk = DB::KeySeq::compare(key_seq, key, (*it.item)->key, 0))
                == Condition::GT 
      || (comp == Condition::GE && chk == Condition::EQ)){
      res.copy((*it.item)->key);
      return true;
    }
  }
  return false;
}

void Mutable::write(DynamicBuffer& cells) const {
  cells.ensure(_bytes);
  for(auto it = ConstIterator(&buckets); it; ++it) {
    if(!(*it.item)->has_expired(ttl))
      (*it.item)->write(cells);
  }
}


void Mutable::scan(ReqScan* req, size_t& skips) const {
  if(!_size)
    return;
  if(max_revs == 1) 
    scan_version_single(req, skips);
  else
    scan_version_multi(req, skips);
}

void Mutable::scan_version_single(ReqScan* req, size_t& skips) const {
  bool stop = false;
  bool only_deletes = req->spec.flags.is_only_deletes();

  size_t offset = req->spec.offset_key.empty() 
                  ? 0 : _narrow(req->spec.offset_key); // ?specs.key_start

  for(auto it = ConstIterator(&buckets, offset); !stop && it; ++it) {
    const Cell& cell = **it.item;
    if(cell.has_expired(ttl) ||
       (only_deletes ? cell.flag == INSERT : cell.flag != INSERT) ||
       !req->selector(key_seq, cell, stop) ||
       req->offset_adjusted()) {
      ++skips;

    } else if(!req->add_cell_and_more(cell)) {
      break;
    }
  }
}

void Mutable::scan_version_multi(ReqScan* req, size_t& skips) const {
  bool stop = false;
  bool only_deletes = req->spec.flags.is_only_deletes();
  
  bool chk_align;
  uint32_t rev;
  size_t offset;
  if(chk_align = !req->spec.offset_key.empty()) {
    rev = req->spec.flags.max_versions;
    offset = _narrow(req->spec.offset_key);// ?req->spec.key_start
   } else {
    rev = 0;
    offset = 0;
  }  

  for(auto it = ConstIterator(&buckets, offset); !stop && it; ++it) {
    const Cell& cell = **it.item;

    if((only_deletes ? cell.flag == INSERT : cell.flag != INSERT) || 
       cell.has_expired(ttl)) {
      ++skips;
      continue;
    }

    if(chk_align) switch(DB::KeySeq::compare(key_seq, 
                                             req->spec.offset_key, cell.key)) {
      case Condition::LT: {
        ++skips;
        continue;
      }
      case Condition::EQ: {
        if(!rev ||
           !req->spec.is_matching(cell.timestamp, cell.control & TS_DESC)) {
          if(rev)
            --rev;
          //if(req->offset && req->selector(cell, stop))
          //  --req->offset;
          ++skips;
          continue;
        }
      }
      default:
        chk_align = false;
        break;
    }

    if(!req->selector(key_seq, cell, stop)) {
      ++skips;
      continue;
    }
    if(req->matching_last(cell.key)) {
      if(!rev) {
        ++skips;
        continue;
      }
    } else {
      rev = req->spec.flags.max_versions;
    }

    if(req->offset_adjusted()) {
      ++skips;
      continue;
    }

    if(!req->add_cell_set_last_and_more(cell))
      break;
    --rev;
  }
}

void Mutable::scan_test_use(const Specs::Interval& specs, 
                            DynamicBuffer& result,
                            size_t& count, size_t& skips) const {
  uint32_t cell_offset = specs.flags.offset;
  bool only_deletes = specs.flags.is_only_deletes();

  for(auto it = ConstIterator(&buckets); it; ++it) {
    const Cell& cell = **it.item;

    if(!cell.has_expired(ttl) && 
       (only_deletes ? cell.flag != INSERT : cell.flag == INSERT) &&
       specs.is_matching(key_seq, cell)) {

      if(cell_offset) {
        --cell_offset;
        ++skips;
        continue;
      }
      
      cell.write(result);
      if(++count == specs.flags.limit) 
        // specs.flags.limit_by && specs.flags.max_versions
          break;
    } else 
      ++skips;
  }
}

void Mutable::scan(Interval& interval, Mutable& cells) const {
  if(!_size)
    return;

  for(auto it=ConstIterator(&buckets, _narrow(interval.key_begin)); it; ++it) {
    const Cell& cell = **it.item;
    if(cell.has_expired(ttl) || (!interval.key_begin.empty() 
        && DB::KeySeq::compare(key_seq, interval.key_begin, cell.key)
            == Condition::LT))
      continue;
    if(!interval.key_end.empty() 
        && DB::KeySeq::compare(key_seq, interval.key_end, cell.key)
            == Condition::GT)
      break; 

    cells.add_raw(cell);
  }
}


void Mutable::expand(Interval& interval) const {
  expand_begin(interval);
  if(size() > 1)
    expand_end(interval);
}

void Mutable::expand_begin(Interval& interval) const {
  interval.expand_begin(*front());
}

void Mutable::expand_end(Interval& interval) const {
  interval.expand_end(*back());
}


void Mutable::split(size_t from, Mutable& cells, 
                    Interval& intval_1st, Interval& intval_2nd,
                    bool loaded) {
  Cell* from_cell = *ConstIterator(&buckets, from).item;
  size_t count = _size;
  bool from_set = false;
  Iterator it_start;
  Cell* cell;
  for(auto it = Iterator(&buckets, _narrow(from_cell->key)); it; ++it) {
    cell=*it.item;

    if(!from_set) {
      if(DB::KeySeq::compare(key_seq, cell->key, from_cell->key, 0)
           == Condition::GT) {
       --count;
        continue;
      }

      it_start = it;
      intval_2nd.expand_begin(*cell);
      if(!loaded)
        break;
      from_set = true;
    }
    _remove(cell);
    if(cell->has_expired(ttl))
      delete cell;
    else
      cells.add_sorted_no_cpy(cell);
  }

  _remove(it_start, count, !loaded);

  intval_2nd.set_key_end(intval_1st.key_end);      
  intval_1st.key_end.free();
  expand_end(intval_1st);
}


void Mutable::_add_remove(const Cell& e_cell, size_t offset) {
  int64_t ts = e_cell.get_timestamp();
  int64_t rev;
  bool chk_rev = (rev = e_cell.get_revision()) != AUTO_ASSIGN;
  Condition::Comp cond;
  int64_t e_ts;
  Cell* cell;
  for(auto it = Iterator(&buckets, offset); it; ) {

    if((cell=*it.item)->has_expired(ttl)) {
      _remove(it);
      continue;
    }

    if((cond = DB::KeySeq::compare(key_seq, cell->key, e_cell.key, 0)) 
                == Condition::GT) {
      ++it;
      continue;
    }

    if(cond == Condition::LT) {
      _insert(it, e_cell);
      return;
    }

    if(cell->is_removing(ts) || 
      (chk_rev && (e_ts = cell->get_revision()) != AUTO_ASSIGN && e_ts >= rev))
      return;
    
    if(e_cell.is_removing(cell->get_timestamp()))
      _remove(it);
    else
      ++it;
  }
  
  add_sorted(e_cell);
}

void Mutable::_add_plain(const Cell& e_cell, size_t offset) {
  int64_t ts;
  bool chk_ts = (ts = e_cell.get_timestamp()) != AUTO_ASSIGN;
  int64_t rev;
  bool chk_rev = (rev = e_cell.get_revision()) != AUTO_ASSIGN;

  uint32_t revs = 0;
  Condition::Comp cond;
  Cell* cell;
  for(auto it = Iterator(&buckets, offset); it;) {

    if((cell=*it.item)->has_expired(ttl)) {
      _remove(it);
      continue;
    }

    if((cond = DB::KeySeq::compare(key_seq, cell->key, e_cell.key, 0))
               == Condition::GT) {
      ++it;
      continue;
    }

    if(cond == Condition::LT) {
      _insert(it, e_cell);
      return;
    }

    if(chk_rev && cell->get_revision() == rev)
      return;

    if(cell->removal()) {
      if(chk_ts && cell->is_removing(ts))
        return;
      ++it;
      continue;
    }

    if(chk_ts && cell->get_timestamp() == ts) {
      if(!chk_rev || rev > cell->get_revision())
        return cell->copy(e_cell);
      return;
    }
    
    ++revs;
    if(e_cell.control & TS_DESC 
        ? (chk_ts && ts < cell->get_timestamp())
        : (!chk_ts || ts > cell->get_timestamp()) ) {
      if(max_revs == revs)
        return;
      ++it;
      continue;
    }
    
    if(max_revs == revs) {
      if(!chk_rev || rev > cell->get_revision())
        cell->copy(e_cell);
    } else {
      _insert(it, e_cell);
      ++it;
      _remove_overhead(it, e_cell.key, revs);
    }
    return;
  }

  add_sorted(e_cell);
}

void Mutable::_add_counter(const Cell& e_cell, size_t offset) {
  Condition::Comp cond;

  int64_t ts = e_cell.get_timestamp();
  int64_t rev;
  bool chk_rev = (rev = e_cell.get_revision()) != AUTO_ASSIGN;
  
  Cell* cell;
  auto it = Iterator(&buckets, offset); 
  while(it) {

    if((cell=*it.item)->has_expired(ttl)) {
      _remove(it);
      continue;
    }

    if((cond = DB::KeySeq::compare(key_seq, cell->key, e_cell.key, 0)) 
                == Condition::GT) { 
      ++it;
      continue;
    }

    if(cond == Condition::LT) //without aggregate|| ts == AUTO_ASSIGN
      goto add_counter;

    if(cell->removal()) {
      if((chk_rev && cell->get_revision() >= rev) ||
         cell->is_removing(ts) )
        return;
      ++it;
      continue;
    }

    uint8_t op_1;
    int64_t eq_rev_1;
    int64_t value_1 = cell->get_counter(op_1, eq_rev_1);
    if(op_1 & OP_EQUAL) {
      if(!(op_1 & HAVE_REVISION))
        eq_rev_1 = cell->get_timestamp();
      if(eq_rev_1 > ts)
        return;
    }
    
    if(e_cell.get_counter_op() & OP_EQUAL)
      cell->copy(e_cell);
    else {
      value_1 += e_cell.get_counter();
      cell->set_counter(op_1, value_1, type, eq_rev_1);
      if(cell->timestamp < e_cell.timestamp) {
        cell->timestamp = e_cell.timestamp;
        cell->control = e_cell.control;
      }
    }
    return;
  }

  add_counter:
    cell = _insert(it, e_cell);
    if(type != Types::Column::COUNTER_I64) {
      uint8_t op_1;
      int64_t eq_rev_1;
      int64_t value_1 = cell->get_counter(op_1, eq_rev_1);
      cell->set_counter(op_1, value_1, type, eq_rev_1);
    }
}


size_t Mutable::_narrow(const DB::Cell::Key& key) const {
  if(key.empty() || _size <= narrow_sz) 
    return 0;
  size_t sz;
  size_t offset = sz = (_size >> 1);

  try_narrow:
    if(DB::KeySeq::compare(
        key_seq, (*ConstIterator(&buckets, offset).item)->key, key, 0)
        == Condition::GT) {
      if(sz < narrow_sz)
        return offset;
      offset += sz >>= 1; 
      goto try_narrow;
    }
    if((sz >>= 1) == 0)
      ++sz;  

    if(offset < sz)
      return 0;
    offset -= sz;
  goto try_narrow;
}


void Mutable::_add(Cell* cell) {
  _bytes += cell->encoded_length();
  ++_size;
}

void Mutable::_remove(Cell* cell) {
  _bytes -= cell->encoded_length();
  --_size;
}


void Mutable::_push_back(Cell* cell) {
  if(buckets.back()->size() >= bucket_size)
    buckets.push_back(make_bucket());
  buckets.back()->push_back(cell);
}

Cell* Mutable::_insert(Mutable::Iterator& it, const Cell& cell) {
  Cell* adding;
  _add(adding = new Cell(cell));
  if(!it) 
    _push_back(adding);
  else 
    it.insert(adding);
  return adding;
}

void Mutable::_remove(Mutable::Iterator& it) {
  _remove(*it.item); 
  delete *it.item;
  it.remove();
}

void Mutable::_remove(Mutable::Iterator& it, size_t number, bool wdel) { 
  if(wdel) {
    auto it_del = Iterator(it);
    for(auto c = number; c && it_del; ++it_del,--c) {
      _remove(*it_del.item);
      delete *it_del.item;
    }
  }
  it.remove(number);
}

void Mutable::_remove_overhead(Mutable::Iterator& it, const DB::Cell::Key& key,
                               uint32_t revs) {
  while(it && DB::KeySeq::compare(key_seq, (*it.item)->key, key, 0) 
                                                  == Condition::EQ) {
    if((*it.item)->flag == INSERT && ++revs > max_revs)
      _remove(it);
    else 
      ++it;
  }
}



Mutable::ConstIterator::ConstIterator(const Mutable::Buckets* buckets, 
                                      size_t offset) : buckets(buckets) {
  bucket = buckets->begin();
  if(offset) {
    for(; bucket < buckets->end(); ++bucket) {
      if(offset < (*bucket)->size()) {
        item = (*bucket)->begin() + offset;
        break;
      }
      offset -= (*bucket)->size();
    }
  } else if(bucket < buckets->end()) {
    item = (*bucket)->begin();
  }
}

Mutable::ConstIterator::ConstIterator(const Mutable::ConstIterator& other)
                                      : buckets(other.buckets), 
                                        bucket(other.bucket), 
                                        item(other.item) {
}

Mutable::ConstIterator::~ConstIterator() { }

bool Mutable::ConstIterator::avail() const {
  return bucket < buckets->end() && item < (*bucket)->end();
}

Mutable::ConstIterator::operator bool() const {
  return avail();
}

void Mutable::ConstIterator::operator++() {
  if(++item == (*bucket)->end() && ++bucket < buckets->end())
    item = (*bucket)->begin();
}




Mutable::Iterator::Iterator() : buckets(nullptr) { }

Mutable::Iterator::Iterator(Mutable::Buckets* buckets, size_t offset) 
                            : buckets(buckets) {
  bucket = buckets->begin();
  if(offset) {
    for(; bucket < buckets->end(); ++bucket) {
      if(offset < (*bucket)->size()) {
        item = (*bucket)->begin() + offset;
        break;
      }
      offset -= (*bucket)->size();
    }
  } else if(bucket < buckets->end()) {
    item = (*bucket)->begin();
  }
}

Mutable::Iterator::Iterator(const Mutable::Iterator& other) 
                            : buckets(other.buckets), 
                              bucket(other.bucket), item(other.item) {
}

Mutable::Iterator& 
Mutable::Iterator::operator=(const Mutable::Iterator& other) {
  buckets = other.buckets;
  bucket = other.bucket;
  item = other.item;
  return *this;
}

Mutable::Iterator::~Iterator() { }

Mutable::Iterator::operator bool() const {
  return avail();
}

bool Mutable::Iterator::avail() const {
  return bucket < buckets->end() && item < (*bucket)->end();
}

void Mutable::Iterator::operator++() {
  if(++item == (*bucket)->end() && ++bucket < buckets->end())
    item = (*bucket)->begin();
}

bool Mutable::Iterator::avail_begin() const {
  return bucket != buckets->begin() && item >= (*bucket)->begin();
}

void Mutable::Iterator::operator--() {
  if(--item == (*bucket)->begin() && --bucket >= buckets->begin())
    item = (*bucket)->end() - 1;
}

void Mutable::Iterator::push_back(Cell*& value) {
  if((*bucket)->size() >= bucket_max)
    buckets->push_back(make_bucket());

  (*bucket)->push_back(value);
  item = (*bucket)->end() - 1;
}

void Mutable::Iterator::insert(Cell*& value) {
  item = (*bucket)->insert(item, value);

  if((*bucket)->size() >= bucket_max) {
    auto offset = item - (*bucket)->begin();

    auto nbucket = buckets->insert(++bucket, make_bucket());

    auto it_b = (*(bucket = nbucket-1))->begin() + bucket_split;
    auto it_e = (*bucket)->end();
    (*nbucket)->assign(it_b, it_e);
    (*bucket)->erase  (it_b, it_e);

    if(offset >= bucket_split)
      item = (*(bucket = nbucket))->begin() + offset - bucket_split;
  }
}

void Mutable::Iterator::remove() {
  (*bucket)->erase(item);
  if((*bucket)->empty() && buckets->size() > 1) {
    delete *bucket; 
    if((bucket = buckets->erase(bucket)) != buckets->end())
      item = (*bucket)->begin();
  } else if(item == (*bucket)->end() && ++bucket < buckets->end()) {
    item = (*bucket)->begin();
  }
}

void Mutable::Iterator::remove(size_t number) {
  while(number) {

    if(item == (*bucket)->begin() && number >= (*bucket)->size()) {
      if(buckets->size() == 1) {
        (*bucket)->clear();
        item = (*bucket)->end();
        return;
      }
      number -= (*bucket)->size();
      delete *bucket;
      bucket = buckets->erase(bucket);

    } else {
      size_t avail;
      if((avail = (*bucket)->end() - item) > number) {
        item = (*bucket)->erase(item, item + number);
        return;
      }
      number -= avail;
      (*bucket)->erase(item, item + avail);
      ++bucket;
    }
    
    if(bucket == buckets->end())
      return;
    item = (*bucket)->begin();
  }
}



}}}
