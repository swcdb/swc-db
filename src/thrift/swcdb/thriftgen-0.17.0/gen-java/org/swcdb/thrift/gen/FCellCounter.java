/**
 * Autogenerated by Thrift Compiler (0.17.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
package org.swcdb.thrift.gen;

/**
 * The Counter column type Fraction Cell for results on Fraction of scan
 */
@SuppressWarnings({"cast", "rawtypes", "serial", "unchecked", "unused"})
public class FCellCounter implements org.apache.thrift.TBase<FCellCounter, FCellCounter._Fields>, java.io.Serializable, Cloneable, Comparable<FCellCounter> {
  private static final org.apache.thrift.protocol.TStruct STRUCT_DESC = new org.apache.thrift.protocol.TStruct("FCellCounter");

  private static final org.apache.thrift.protocol.TField C_FIELD_DESC = new org.apache.thrift.protocol.TField("c", org.apache.thrift.protocol.TType.STRING, (short)1);
  private static final org.apache.thrift.protocol.TField TS_FIELD_DESC = new org.apache.thrift.protocol.TField("ts", org.apache.thrift.protocol.TType.I64, (short)2);
  private static final org.apache.thrift.protocol.TField V_FIELD_DESC = new org.apache.thrift.protocol.TField("v", org.apache.thrift.protocol.TType.I64, (short)3);
  private static final org.apache.thrift.protocol.TField EQ_FIELD_DESC = new org.apache.thrift.protocol.TField("eq", org.apache.thrift.protocol.TType.I64, (short)4);

  private static final org.apache.thrift.scheme.SchemeFactory STANDARD_SCHEME_FACTORY = new FCellCounterStandardSchemeFactory();
  private static final org.apache.thrift.scheme.SchemeFactory TUPLE_SCHEME_FACTORY = new FCellCounterTupleSchemeFactory();

  /**
   * The Column Name
   */
  public @org.apache.thrift.annotation.Nullable java.lang.String c; // required
  /**
   * The Cell Timestamp
   */
  public long ts; // required
  /**
   * The Cell Counter Value
   */
  public long v; // required
  /**
   * The Counter EQ since ts
   */
  public long eq; // optional

  /** The set of fields this struct contains, along with convenience methods for finding and manipulating them. */
  public enum _Fields implements org.apache.thrift.TFieldIdEnum {
    /**
     * The Column Name
     */
    C((short)1, "c"),
    /**
     * The Cell Timestamp
     */
    TS((short)2, "ts"),
    /**
     * The Cell Counter Value
     */
    V((short)3, "v"),
    /**
     * The Counter EQ since ts
     */
    EQ((short)4, "eq");

    private static final java.util.Map<java.lang.String, _Fields> byName = new java.util.HashMap<java.lang.String, _Fields>();

    static {
      for (_Fields field : java.util.EnumSet.allOf(_Fields.class)) {
        byName.put(field.getFieldName(), field);
      }
    }

    /**
     * Find the _Fields constant that matches fieldId, or null if its not found.
     */
    @org.apache.thrift.annotation.Nullable
    public static _Fields findByThriftId(int fieldId) {
      switch(fieldId) {
        case 1: // C
          return C;
        case 2: // TS
          return TS;
        case 3: // V
          return V;
        case 4: // EQ
          return EQ;
        default:
          return null;
      }
    }

    /**
     * Find the _Fields constant that matches fieldId, throwing an exception
     * if it is not found.
     */
    public static _Fields findByThriftIdOrThrow(int fieldId) {
      _Fields fields = findByThriftId(fieldId);
      if (fields == null) throw new java.lang.IllegalArgumentException("Field " + fieldId + " doesn't exist!");
      return fields;
    }

    /**
     * Find the _Fields constant that matches name, or null if its not found.
     */
    @org.apache.thrift.annotation.Nullable
    public static _Fields findByName(java.lang.String name) {
      return byName.get(name);
    }

    private final short _thriftId;
    private final java.lang.String _fieldName;

    _Fields(short thriftId, java.lang.String fieldName) {
      _thriftId = thriftId;
      _fieldName = fieldName;
    }

    @Override
    public short getThriftFieldId() {
      return _thriftId;
    }

    @Override
    public java.lang.String getFieldName() {
      return _fieldName;
    }
  }

  // isset id assignments
  private static final int __TS_ISSET_ID = 0;
  private static final int __V_ISSET_ID = 1;
  private static final int __EQ_ISSET_ID = 2;
  private byte __isset_bitfield = 0;
  private static final _Fields optionals[] = {_Fields.EQ};
  public static final java.util.Map<_Fields, org.apache.thrift.meta_data.FieldMetaData> metaDataMap;
  static {
    java.util.Map<_Fields, org.apache.thrift.meta_data.FieldMetaData> tmpMap = new java.util.EnumMap<_Fields, org.apache.thrift.meta_data.FieldMetaData>(_Fields.class);
    tmpMap.put(_Fields.C, new org.apache.thrift.meta_data.FieldMetaData("c", org.apache.thrift.TFieldRequirementType.DEFAULT, 
        new org.apache.thrift.meta_data.FieldValueMetaData(org.apache.thrift.protocol.TType.STRING)));
    tmpMap.put(_Fields.TS, new org.apache.thrift.meta_data.FieldMetaData("ts", org.apache.thrift.TFieldRequirementType.DEFAULT, 
        new org.apache.thrift.meta_data.FieldValueMetaData(org.apache.thrift.protocol.TType.I64)));
    tmpMap.put(_Fields.V, new org.apache.thrift.meta_data.FieldMetaData("v", org.apache.thrift.TFieldRequirementType.DEFAULT, 
        new org.apache.thrift.meta_data.FieldValueMetaData(org.apache.thrift.protocol.TType.I64)));
    tmpMap.put(_Fields.EQ, new org.apache.thrift.meta_data.FieldMetaData("eq", org.apache.thrift.TFieldRequirementType.OPTIONAL, 
        new org.apache.thrift.meta_data.FieldValueMetaData(org.apache.thrift.protocol.TType.I64)));
    metaDataMap = java.util.Collections.unmodifiableMap(tmpMap);
    org.apache.thrift.meta_data.FieldMetaData.addStructMetaDataMap(FCellCounter.class, metaDataMap);
  }

  public FCellCounter() {
  }

  public FCellCounter(
    java.lang.String c,
    long ts,
    long v)
  {
    this();
    this.c = c;
    this.ts = ts;
    setTsIsSet(true);
    this.v = v;
    setVIsSet(true);
  }

  /**
   * Performs a deep copy on <i>other</i>.
   */
  public FCellCounter(FCellCounter other) {
    __isset_bitfield = other.__isset_bitfield;
    if (other.isSetC()) {
      this.c = other.c;
    }
    this.ts = other.ts;
    this.v = other.v;
    this.eq = other.eq;
  }

  @Override
  public FCellCounter deepCopy() {
    return new FCellCounter(this);
  }

  @Override
  public void clear() {
    this.c = null;
    setTsIsSet(false);
    this.ts = 0;
    setVIsSet(false);
    this.v = 0;
    setEqIsSet(false);
    this.eq = 0;
  }

  /**
   * The Column Name
   */
  @org.apache.thrift.annotation.Nullable
  public java.lang.String getC() {
    return this.c;
  }

  /**
   * The Column Name
   */
  public FCellCounter setC(@org.apache.thrift.annotation.Nullable java.lang.String c) {
    this.c = c;
    return this;
  }

  public void unsetC() {
    this.c = null;
  }

  /** Returns true if field c is set (has been assigned a value) and false otherwise */
  public boolean isSetC() {
    return this.c != null;
  }

  public void setCIsSet(boolean value) {
    if (!value) {
      this.c = null;
    }
  }

  /**
   * The Cell Timestamp
   */
  public long getTs() {
    return this.ts;
  }

  /**
   * The Cell Timestamp
   */
  public FCellCounter setTs(long ts) {
    this.ts = ts;
    setTsIsSet(true);
    return this;
  }

  public void unsetTs() {
    __isset_bitfield = org.apache.thrift.EncodingUtils.clearBit(__isset_bitfield, __TS_ISSET_ID);
  }

  /** Returns true if field ts is set (has been assigned a value) and false otherwise */
  public boolean isSetTs() {
    return org.apache.thrift.EncodingUtils.testBit(__isset_bitfield, __TS_ISSET_ID);
  }

  public void setTsIsSet(boolean value) {
    __isset_bitfield = org.apache.thrift.EncodingUtils.setBit(__isset_bitfield, __TS_ISSET_ID, value);
  }

  /**
   * The Cell Counter Value
   */
  public long getV() {
    return this.v;
  }

  /**
   * The Cell Counter Value
   */
  public FCellCounter setV(long v) {
    this.v = v;
    setVIsSet(true);
    return this;
  }

  public void unsetV() {
    __isset_bitfield = org.apache.thrift.EncodingUtils.clearBit(__isset_bitfield, __V_ISSET_ID);
  }

  /** Returns true if field v is set (has been assigned a value) and false otherwise */
  public boolean isSetV() {
    return org.apache.thrift.EncodingUtils.testBit(__isset_bitfield, __V_ISSET_ID);
  }

  public void setVIsSet(boolean value) {
    __isset_bitfield = org.apache.thrift.EncodingUtils.setBit(__isset_bitfield, __V_ISSET_ID, value);
  }

  /**
   * The Counter EQ since ts
   */
  public long getEq() {
    return this.eq;
  }

  /**
   * The Counter EQ since ts
   */
  public FCellCounter setEq(long eq) {
    this.eq = eq;
    setEqIsSet(true);
    return this;
  }

  public void unsetEq() {
    __isset_bitfield = org.apache.thrift.EncodingUtils.clearBit(__isset_bitfield, __EQ_ISSET_ID);
  }

  /** Returns true if field eq is set (has been assigned a value) and false otherwise */
  public boolean isSetEq() {
    return org.apache.thrift.EncodingUtils.testBit(__isset_bitfield, __EQ_ISSET_ID);
  }

  public void setEqIsSet(boolean value) {
    __isset_bitfield = org.apache.thrift.EncodingUtils.setBit(__isset_bitfield, __EQ_ISSET_ID, value);
  }

  @Override
  public void setFieldValue(_Fields field, @org.apache.thrift.annotation.Nullable java.lang.Object value) {
    switch (field) {
    case C:
      if (value == null) {
        unsetC();
      } else {
        setC((java.lang.String)value);
      }
      break;

    case TS:
      if (value == null) {
        unsetTs();
      } else {
        setTs((java.lang.Long)value);
      }
      break;

    case V:
      if (value == null) {
        unsetV();
      } else {
        setV((java.lang.Long)value);
      }
      break;

    case EQ:
      if (value == null) {
        unsetEq();
      } else {
        setEq((java.lang.Long)value);
      }
      break;

    }
  }

  @org.apache.thrift.annotation.Nullable
  @Override
  public java.lang.Object getFieldValue(_Fields field) {
    switch (field) {
    case C:
      return getC();

    case TS:
      return getTs();

    case V:
      return getV();

    case EQ:
      return getEq();

    }
    throw new java.lang.IllegalStateException();
  }

  /** Returns true if field corresponding to fieldID is set (has been assigned a value) and false otherwise */
  @Override
  public boolean isSet(_Fields field) {
    if (field == null) {
      throw new java.lang.IllegalArgumentException();
    }

    switch (field) {
    case C:
      return isSetC();
    case TS:
      return isSetTs();
    case V:
      return isSetV();
    case EQ:
      return isSetEq();
    }
    throw new java.lang.IllegalStateException();
  }

  @Override
  public boolean equals(java.lang.Object that) {
    if (that instanceof FCellCounter)
      return this.equals((FCellCounter)that);
    return false;
  }

  public boolean equals(FCellCounter that) {
    if (that == null)
      return false;
    if (this == that)
      return true;

    boolean this_present_c = true && this.isSetC();
    boolean that_present_c = true && that.isSetC();
    if (this_present_c || that_present_c) {
      if (!(this_present_c && that_present_c))
        return false;
      if (!this.c.equals(that.c))
        return false;
    }

    boolean this_present_ts = true;
    boolean that_present_ts = true;
    if (this_present_ts || that_present_ts) {
      if (!(this_present_ts && that_present_ts))
        return false;
      if (this.ts != that.ts)
        return false;
    }

    boolean this_present_v = true;
    boolean that_present_v = true;
    if (this_present_v || that_present_v) {
      if (!(this_present_v && that_present_v))
        return false;
      if (this.v != that.v)
        return false;
    }

    boolean this_present_eq = true && this.isSetEq();
    boolean that_present_eq = true && that.isSetEq();
    if (this_present_eq || that_present_eq) {
      if (!(this_present_eq && that_present_eq))
        return false;
      if (this.eq != that.eq)
        return false;
    }

    return true;
  }

  @Override
  public int hashCode() {
    int hashCode = 1;

    hashCode = hashCode * 8191 + ((isSetC()) ? 131071 : 524287);
    if (isSetC())
      hashCode = hashCode * 8191 + c.hashCode();

    hashCode = hashCode * 8191 + org.apache.thrift.TBaseHelper.hashCode(ts);

    hashCode = hashCode * 8191 + org.apache.thrift.TBaseHelper.hashCode(v);

    hashCode = hashCode * 8191 + ((isSetEq()) ? 131071 : 524287);
    if (isSetEq())
      hashCode = hashCode * 8191 + org.apache.thrift.TBaseHelper.hashCode(eq);

    return hashCode;
  }

  @Override
  public int compareTo(FCellCounter other) {
    if (!getClass().equals(other.getClass())) {
      return getClass().getName().compareTo(other.getClass().getName());
    }

    int lastComparison = 0;

    lastComparison = java.lang.Boolean.compare(isSetC(), other.isSetC());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetC()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.c, other.c);
      if (lastComparison != 0) {
        return lastComparison;
      }
    }
    lastComparison = java.lang.Boolean.compare(isSetTs(), other.isSetTs());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetTs()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.ts, other.ts);
      if (lastComparison != 0) {
        return lastComparison;
      }
    }
    lastComparison = java.lang.Boolean.compare(isSetV(), other.isSetV());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetV()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.v, other.v);
      if (lastComparison != 0) {
        return lastComparison;
      }
    }
    lastComparison = java.lang.Boolean.compare(isSetEq(), other.isSetEq());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetEq()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.eq, other.eq);
      if (lastComparison != 0) {
        return lastComparison;
      }
    }
    return 0;
  }

  @org.apache.thrift.annotation.Nullable
  @Override
  public _Fields fieldForId(int fieldId) {
    return _Fields.findByThriftId(fieldId);
  }

  @Override
  public void read(org.apache.thrift.protocol.TProtocol iprot) throws org.apache.thrift.TException {
    scheme(iprot).read(iprot, this);
  }

  @Override
  public void write(org.apache.thrift.protocol.TProtocol oprot) throws org.apache.thrift.TException {
    scheme(oprot).write(oprot, this);
  }

  @Override
  public java.lang.String toString() {
    java.lang.StringBuilder sb = new java.lang.StringBuilder("FCellCounter(");
    boolean first = true;

    sb.append("c:");
    if (this.c == null) {
      sb.append("null");
    } else {
      sb.append(this.c);
    }
    first = false;
    if (!first) sb.append(", ");
    sb.append("ts:");
    sb.append(this.ts);
    first = false;
    if (!first) sb.append(", ");
    sb.append("v:");
    sb.append(this.v);
    first = false;
    if (isSetEq()) {
      if (!first) sb.append(", ");
      sb.append("eq:");
      sb.append(this.eq);
      first = false;
    }
    sb.append(")");
    return sb.toString();
  }

  public void validate() throws org.apache.thrift.TException {
    // check for required fields
    // check for sub-struct validity
  }

  private void writeObject(java.io.ObjectOutputStream out) throws java.io.IOException {
    try {
      write(new org.apache.thrift.protocol.TCompactProtocol(new org.apache.thrift.transport.TIOStreamTransport(out)));
    } catch (org.apache.thrift.TException te) {
      throw new java.io.IOException(te);
    }
  }

  private void readObject(java.io.ObjectInputStream in) throws java.io.IOException, java.lang.ClassNotFoundException {
    try {
      // it doesn't seem like you should have to do this, but java serialization is wacky, and doesn't call the default constructor.
      __isset_bitfield = 0;
      read(new org.apache.thrift.protocol.TCompactProtocol(new org.apache.thrift.transport.TIOStreamTransport(in)));
    } catch (org.apache.thrift.TException te) {
      throw new java.io.IOException(te);
    }
  }

  private static class FCellCounterStandardSchemeFactory implements org.apache.thrift.scheme.SchemeFactory {
    @Override
    public FCellCounterStandardScheme getScheme() {
      return new FCellCounterStandardScheme();
    }
  }

  private static class FCellCounterStandardScheme extends org.apache.thrift.scheme.StandardScheme<FCellCounter> {

    @Override
    public void read(org.apache.thrift.protocol.TProtocol iprot, FCellCounter struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TField schemeField;
      iprot.readStructBegin();
      while (true)
      {
        schemeField = iprot.readFieldBegin();
        if (schemeField.type == org.apache.thrift.protocol.TType.STOP) { 
          break;
        }
        switch (schemeField.id) {
          case 1: // C
            if (schemeField.type == org.apache.thrift.protocol.TType.STRING) {
              struct.c = iprot.readString();
              struct.setCIsSet(true);
            } else { 
              org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
            }
            break;
          case 2: // TS
            if (schemeField.type == org.apache.thrift.protocol.TType.I64) {
              struct.ts = iprot.readI64();
              struct.setTsIsSet(true);
            } else { 
              org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
            }
            break;
          case 3: // V
            if (schemeField.type == org.apache.thrift.protocol.TType.I64) {
              struct.v = iprot.readI64();
              struct.setVIsSet(true);
            } else { 
              org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
            }
            break;
          case 4: // EQ
            if (schemeField.type == org.apache.thrift.protocol.TType.I64) {
              struct.eq = iprot.readI64();
              struct.setEqIsSet(true);
            } else { 
              org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
            }
            break;
          default:
            org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
        }
        iprot.readFieldEnd();
      }
      iprot.readStructEnd();

      // check for required fields of primitive type, which can't be checked in the validate method
      struct.validate();
    }

    @Override
    public void write(org.apache.thrift.protocol.TProtocol oprot, FCellCounter struct) throws org.apache.thrift.TException {
      struct.validate();

      oprot.writeStructBegin(STRUCT_DESC);
      if (struct.c != null) {
        oprot.writeFieldBegin(C_FIELD_DESC);
        oprot.writeString(struct.c);
        oprot.writeFieldEnd();
      }
      oprot.writeFieldBegin(TS_FIELD_DESC);
      oprot.writeI64(struct.ts);
      oprot.writeFieldEnd();
      oprot.writeFieldBegin(V_FIELD_DESC);
      oprot.writeI64(struct.v);
      oprot.writeFieldEnd();
      if (struct.isSetEq()) {
        oprot.writeFieldBegin(EQ_FIELD_DESC);
        oprot.writeI64(struct.eq);
        oprot.writeFieldEnd();
      }
      oprot.writeFieldStop();
      oprot.writeStructEnd();
    }

  }

  private static class FCellCounterTupleSchemeFactory implements org.apache.thrift.scheme.SchemeFactory {
    @Override
    public FCellCounterTupleScheme getScheme() {
      return new FCellCounterTupleScheme();
    }
  }

  private static class FCellCounterTupleScheme extends org.apache.thrift.scheme.TupleScheme<FCellCounter> {

    @Override
    public void write(org.apache.thrift.protocol.TProtocol prot, FCellCounter struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TTupleProtocol oprot = (org.apache.thrift.protocol.TTupleProtocol) prot;
      java.util.BitSet optionals = new java.util.BitSet();
      if (struct.isSetC()) {
        optionals.set(0);
      }
      if (struct.isSetTs()) {
        optionals.set(1);
      }
      if (struct.isSetV()) {
        optionals.set(2);
      }
      if (struct.isSetEq()) {
        optionals.set(3);
      }
      oprot.writeBitSet(optionals, 4);
      if (struct.isSetC()) {
        oprot.writeString(struct.c);
      }
      if (struct.isSetTs()) {
        oprot.writeI64(struct.ts);
      }
      if (struct.isSetV()) {
        oprot.writeI64(struct.v);
      }
      if (struct.isSetEq()) {
        oprot.writeI64(struct.eq);
      }
    }

    @Override
    public void read(org.apache.thrift.protocol.TProtocol prot, FCellCounter struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TTupleProtocol iprot = (org.apache.thrift.protocol.TTupleProtocol) prot;
      java.util.BitSet incoming = iprot.readBitSet(4);
      if (incoming.get(0)) {
        struct.c = iprot.readString();
        struct.setCIsSet(true);
      }
      if (incoming.get(1)) {
        struct.ts = iprot.readI64();
        struct.setTsIsSet(true);
      }
      if (incoming.get(2)) {
        struct.v = iprot.readI64();
        struct.setVIsSet(true);
      }
      if (incoming.get(3)) {
        struct.eq = iprot.readI64();
        struct.setEqIsSet(true);
      }
    }
  }

  private static <S extends org.apache.thrift.scheme.IScheme> S scheme(org.apache.thrift.protocol.TProtocol proto) {
    return (org.apache.thrift.scheme.StandardScheme.class.equals(proto.getScheme()) ? STANDARD_SCHEME_FACTORY : TUPLE_SCHEME_FACTORY).getScheme();
  }
}

