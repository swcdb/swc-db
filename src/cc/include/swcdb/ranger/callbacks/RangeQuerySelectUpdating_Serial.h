/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */

#ifndef swcdb_ranger_callbacks_RangeQuerySelectUpdating_Serial_h
#define swcdb_ranger_callbacks_RangeQuerySelectUpdating_Serial_h


#include "swcdb/ranger/callbacks/RangeQuerySelectUpdating.h"


namespace SWC { namespace Ranger { namespace Callback {


class RangeQuerySelectUpdating_Serial final
    : public RangeQuerySelectUpdating {
  public:

  typedef std::shared_ptr<RangeQuerySelectUpdating_Serial> Ptr;

  SWC_CAN_INLINE
  RangeQuerySelectUpdating_Serial(const Comm::ConnHandlerPtr& conn,
                                  const Comm::Event::Ptr& ev,
                                  DB::Specs::Interval&& req_spec,
                                  const RangePtr& a_range)
                                  : RangeQuerySelectUpdating(
                                      conn,
                                      ev,
                                      std::move(req_spec),
                                      a_range
                                    ),
                                    u_fields(
                                      spec.updating->value,
                                      spec.updating->vlen,
                                      false
                                    ) {
    only_keys = false;
  }

  virtual ~RangeQuerySelectUpdating_Serial() noexcept { }

  void update_cell_value(DB::Cells::Cell& cell) override {
    // TODO: (it is only rewrites and fill not-found)

    const uint8_t* ptr = cell.value;
    size_t remain = cell.vlen;

    DB::Cell::Serial::Value::FieldsWriter wfields;
    wfields.ensure(cell.vlen + spec.updating->vlen);

    DB::Cell::Serial::Value::FieldUpdateOpPtrs found_u_fields;
    found_u_fields.reserve(u_fields.count);

    while(remain) {
      switch(DB::Cell::Serial::Value::read_type(&ptr, &remain)) {
        case DB::Cell::Serial::Value::Type::INT64: {
          DB::Cell::Serial::Value::Field_INT64 field(&ptr, &remain);
          auto opfield = u_fields.find_matching_type_and_id(&field);
          if(opfield) {
            found_u_fields.push_back(opfield);
            reinterpret_cast<DB::Cell::Serial::Value::FieldUpdate_MATH*>(
              opfield->ufield)->apply(opfield->field, field);
          }
          wfields.add(&field);
          break;
        }
        case DB::Cell::Serial::Value::Type::DOUBLE: {
          DB::Cell::Serial::Value::Field_DOUBLE field(&ptr, &remain);
          auto opfield = u_fields.find_matching_type_and_id(&field);
          if(opfield) {
            found_u_fields.push_back(opfield);
            reinterpret_cast<DB::Cell::Serial::Value::FieldUpdate_MATH*>(
              opfield->ufield)->apply(opfield->field, field);
          }
          wfields.add(&field);
          break;
        }
        case DB::Cell::Serial::Value::Type::BYTES: {
          DB::Cell::Serial::Value::Field_BYTES field(&ptr, &remain);
          auto opfield = u_fields.find_matching_type_and_id(&field);
          if(opfield) {
            found_u_fields.push_back(opfield);

          }
          wfields.add(&field);
          break;
        }
        case DB::Cell::Serial::Value::Type::KEY: {
          DB::Cell::Serial::Value::Field_KEY field(&ptr, &remain);
          auto opfield = u_fields.find_matching_type_and_id(&field);
          if(opfield) {
            found_u_fields.push_back(opfield);

          }
          wfields.add(&field);
          break;
        }
        case DB::Cell::Serial::Value::Type::LIST_INT64: {
          DB::Cell::Serial::Value::Field_LIST_INT64 field(&ptr, &remain);
          auto opfield = u_fields.find_matching_type_and_id(&field);
          if(opfield) {
            found_u_fields.push_back(opfield);

          }
          wfields.add(&field);
          break;
        }
        case DB::Cell::Serial::Value::Type::LIST_BYTES: {
          DB::Cell::Serial::Value::Field_LIST_BYTES field(&ptr, &remain);
          auto opfield = u_fields.find_matching_type_and_id(&field);
          if(opfield) {
            found_u_fields.push_back(opfield);

          }
          wfields.add(&field);
          break;
        }
        default:
          continue;
      }
    }

    auto missing_u_fields(u_fields.get_not_in(found_u_fields));
    for(auto opfield : missing_u_fields) {
      if(!opfield->ufield->without_adding_field())
        wfields.add(opfield->field);
    }

    cell.set_value(wfields.base, wfields.fill(), true);
  }

  private:
  const DB::Cell::Serial::Value::FieldsUpdaterMap u_fields;

};


}}}
#endif // swcdb_ranger_callbacks_RangeQuerySelectUpdating_Serial_h
