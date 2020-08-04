
/*
 * Copyright Since 2019 SWC-DB© [author: Kashirin Alex kashirin.alex@gmail.com]
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */

#ifndef swc_db_protocol_rgr_params_Report_h
#define swc_db_protocol_rgr_params_Report_h


#include "swcdb/core/comm/ClientConnQueue.h"
#include "swcdb/core/Serializable.h"
#include "swcdb/db/Cells/Interval.h"


namespace SWC { namespace Protocol { namespace Rgr { namespace Params {



class ReportReq : public Serializable {
  public:
  
  static const uint8_t RANGES     = 0x1;
  static const uint8_t COLUMNS    = 0x2;
  static const uint8_t COLUMN     = 0x4;
  static const uint8_t RESOURCES  = 0x8;

  ReportReq(uint8_t flags = COLUMNS);

  ReportReq(uint8_t flags, cid_t cid);

  virtual ~ReportReq();

  uint8_t flags;
  cid_t   cid;

  private:

  size_t internal_encoded_length() const;
    
  void internal_encode(uint8_t** bufp) const;
    
  void internal_decode(const uint8_t** bufp, size_t* remainp);

};



class ReportRsp  : public Serializable {
  public:
  
  struct Range {

    Range(Types::KeySeq seq);

    static bool before(Range* r1, Range* r2);

    rid_t               rid;
    DB::Cells::Interval interval;

    ~Range();

    size_t encoded_length () const;

    void encode(uint8_t** bufp) const;

    void decode(const uint8_t** bufp, size_t* remainp);
  
    void display(std::ostream& out, bool pretty=true, 
                 std::string offset = "") const;
  };

  struct Column {

    static bool before(Column* c1, Column* c2);

    cid_t                cid;
    Types::KeySeq        col_seq;
    uint64_t             mem_bytes;
    std::vector<Range*>  ranges;

    ~Column();

    size_t encoded_length () const;

    void encode(uint8_t** bufp) const;

    void decode(const uint8_t** bufp, size_t* remainp);
    
    void display(std::ostream& out, bool pretty=true, 
                 std::string offset = "") const;
  };


  explicit ReportRsp(int err=Error::OK);

  ReportRsp& operator=(const ReportRsp& other) = delete;

  virtual ~ReportRsp();

  int                  err; 
  rgrid_t              rgrid; 
  EndPoints            endpoints;
  std::vector<Column*> columns;

  void display(std::ostream& out, bool pretty=true, 
               std::string offset = "") const;

  private:

  size_t internal_encoded_length() const;

  void internal_encode(uint8_t** bufp) const;
    
  void internal_decode(const uint8_t** bufp, size_t* remainp);

};



class ReportResRsp  : public Serializable {
  public:
  
  explicit ReportResRsp(int err=Error::OK);

  virtual ~ReportResRsp();

  int         err; 
  uint32_t    mem;
  uint32_t    cpu;
  size_t      ranges;

  void display(std::ostream& out, const std::string& offset="") const;

  private:

  size_t internal_encoded_length() const;

  void internal_encode(uint8_t** bufp) const;
    
  void internal_decode(const uint8_t** bufp, size_t* remainp);

};



}}}}

#ifdef SWC_IMPL_SOURCE
#include "swcdb/db/Protocol/Rgr/params/Report.cc"
#endif 

#endif // swc_db_protocol_rgr_params_Report_h
