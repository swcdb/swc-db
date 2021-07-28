/**
 * Autogenerated by Thrift Compiler (0.14.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
package org.swcdb.thrift.gen;

@SuppressWarnings({"cast", "rawtypes", "serial", "unchecked", "unused"})
/**
 * The Specifications of KEY Serial Value Field
 */
public class SpecValueSerial_KEY implements org.apache.thrift.TBase<SpecValueSerial_KEY, SpecValueSerial_KEY._Fields>, java.io.Serializable, Cloneable, Comparable<SpecValueSerial_KEY> {
  private static final org.apache.thrift.protocol.TStruct STRUCT_DESC = new org.apache.thrift.protocol.TStruct("SpecValueSerial_KEY");

  private static final org.apache.thrift.protocol.TField SEQ_FIELD_DESC = new org.apache.thrift.protocol.TField("seq", org.apache.thrift.protocol.TType.I32, (short)1);
  private static final org.apache.thrift.protocol.TField V_FIELD_DESC = new org.apache.thrift.protocol.TField("v", org.apache.thrift.protocol.TType.LIST, (short)2);

  private static final org.apache.thrift.scheme.SchemeFactory STANDARD_SCHEME_FACTORY = new SpecValueSerial_KEYStandardSchemeFactory();
  private static final org.apache.thrift.scheme.SchemeFactory TUPLE_SCHEME_FACTORY = new SpecValueSerial_KEYTupleSchemeFactory();

  /**
   * The Key Sequence to use
   * 
   * @see KeySeq
   */
  public @org.apache.thrift.annotation.Nullable KeySeq seq; // required
  /**
   * The Specification of the Key to match against the value field
   */
  public @org.apache.thrift.annotation.Nullable java.util.List<SpecFraction> v; // required

  /** The set of fields this struct contains, along with convenience methods for finding and manipulating them. */
  public enum _Fields implements org.apache.thrift.TFieldIdEnum {
    /**
     * The Key Sequence to use
     * 
     * @see KeySeq
     */
    SEQ((short)1, "seq"),
    /**
     * The Specification of the Key to match against the value field
     */
    V((short)2, "v");

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
        case 1: // SEQ
          return SEQ;
        case 2: // V
          return V;
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

    public short getThriftFieldId() {
      return _thriftId;
    }

    public java.lang.String getFieldName() {
      return _fieldName;
    }
  }

  // isset id assignments
  public static final java.util.Map<_Fields, org.apache.thrift.meta_data.FieldMetaData> metaDataMap;
  static {
    java.util.Map<_Fields, org.apache.thrift.meta_data.FieldMetaData> tmpMap = new java.util.EnumMap<_Fields, org.apache.thrift.meta_data.FieldMetaData>(_Fields.class);
    tmpMap.put(_Fields.SEQ, new org.apache.thrift.meta_data.FieldMetaData("seq", org.apache.thrift.TFieldRequirementType.DEFAULT, 
        new org.apache.thrift.meta_data.EnumMetaData(org.apache.thrift.protocol.TType.ENUM, KeySeq.class)));
    tmpMap.put(_Fields.V, new org.apache.thrift.meta_data.FieldMetaData("v", org.apache.thrift.TFieldRequirementType.DEFAULT, 
        new org.apache.thrift.meta_data.FieldValueMetaData(org.apache.thrift.protocol.TType.LIST        , "SpecKey")));
    metaDataMap = java.util.Collections.unmodifiableMap(tmpMap);
    org.apache.thrift.meta_data.FieldMetaData.addStructMetaDataMap(SpecValueSerial_KEY.class, metaDataMap);
  }

  public SpecValueSerial_KEY() {
  }

  public SpecValueSerial_KEY(
    KeySeq seq,
    java.util.List<SpecFraction> v)
  {
    this();
    this.seq = seq;
    this.v = v;
  }

  /**
   * Performs a deep copy on <i>other</i>.
   */
  public SpecValueSerial_KEY(SpecValueSerial_KEY other) {
    if (other.isSetSeq()) {
      this.seq = other.seq;
    }
    if (other.isSetV()) {
      java.util.List<SpecFraction> __this__v = new java.util.ArrayList<SpecFraction>(other.v.size());
      for (SpecFraction other_element : other.v) {
        __this__v.add(new SpecFraction(other_element));
      }
      this.v = __this__v;
    }
  }

  public SpecValueSerial_KEY deepCopy() {
    return new SpecValueSerial_KEY(this);
  }

  @Override
  public void clear() {
    this.seq = null;
    this.v = null;
  }

  /**
   * The Key Sequence to use
   * 
   * @see KeySeq
   */
  @org.apache.thrift.annotation.Nullable
  public KeySeq getSeq() {
    return this.seq;
  }

  /**
   * The Key Sequence to use
   * 
   * @see KeySeq
   */
  public SpecValueSerial_KEY setSeq(@org.apache.thrift.annotation.Nullable KeySeq seq) {
    this.seq = seq;
    return this;
  }

  public void unsetSeq() {
    this.seq = null;
  }

  /** Returns true if field seq is set (has been assigned a value) and false otherwise */
  public boolean isSetSeq() {
    return this.seq != null;
  }

  public void setSeqIsSet(boolean value) {
    if (!value) {
      this.seq = null;
    }
  }

  public int getVSize() {
    return (this.v == null) ? 0 : this.v.size();
  }

  @org.apache.thrift.annotation.Nullable
  public java.util.Iterator<SpecFraction> getVIterator() {
    return (this.v == null) ? null : this.v.iterator();
  }

  public void addToV(SpecFraction elem) {
    if (this.v == null) {
      this.v = new java.util.ArrayList<SpecFraction>();
    }
    this.v.add(elem);
  }

  /**
   * The Specification of the Key to match against the value field
   */
  @org.apache.thrift.annotation.Nullable
  public java.util.List<SpecFraction> getV() {
    return this.v;
  }

  /**
   * The Specification of the Key to match against the value field
   */
  public SpecValueSerial_KEY setV(@org.apache.thrift.annotation.Nullable java.util.List<SpecFraction> v) {
    this.v = v;
    return this;
  }

  public void unsetV() {
    this.v = null;
  }

  /** Returns true if field v is set (has been assigned a value) and false otherwise */
  public boolean isSetV() {
    return this.v != null;
  }

  public void setVIsSet(boolean value) {
    if (!value) {
      this.v = null;
    }
  }

  public void setFieldValue(_Fields field, @org.apache.thrift.annotation.Nullable java.lang.Object value) {
    switch (field) {
    case SEQ:
      if (value == null) {
        unsetSeq();
      } else {
        setSeq((KeySeq)value);
      }
      break;

    case V:
      if (value == null) {
        unsetV();
      } else {
        setV((java.util.List<SpecFraction>)value);
      }
      break;

    }
  }

  @org.apache.thrift.annotation.Nullable
  public java.lang.Object getFieldValue(_Fields field) {
    switch (field) {
    case SEQ:
      return getSeq();

    case V:
      return getV();

    }
    throw new java.lang.IllegalStateException();
  }

  /** Returns true if field corresponding to fieldID is set (has been assigned a value) and false otherwise */
  public boolean isSet(_Fields field) {
    if (field == null) {
      throw new java.lang.IllegalArgumentException();
    }

    switch (field) {
    case SEQ:
      return isSetSeq();
    case V:
      return isSetV();
    }
    throw new java.lang.IllegalStateException();
  }

  @Override
  public boolean equals(java.lang.Object that) {
    if (that instanceof SpecValueSerial_KEY)
      return this.equals((SpecValueSerial_KEY)that);
    return false;
  }

  public boolean equals(SpecValueSerial_KEY that) {
    if (that == null)
      return false;
    if (this == that)
      return true;

    boolean this_present_seq = true && this.isSetSeq();
    boolean that_present_seq = true && that.isSetSeq();
    if (this_present_seq || that_present_seq) {
      if (!(this_present_seq && that_present_seq))
        return false;
      if (!this.seq.equals(that.seq))
        return false;
    }

    boolean this_present_v = true && this.isSetV();
    boolean that_present_v = true && that.isSetV();
    if (this_present_v || that_present_v) {
      if (!(this_present_v && that_present_v))
        return false;
      if (!this.v.equals(that.v))
        return false;
    }

    return true;
  }

  @Override
  public int hashCode() {
    int hashCode = 1;

    hashCode = hashCode * 8191 + ((isSetSeq()) ? 131071 : 524287);
    if (isSetSeq())
      hashCode = hashCode * 8191 + seq.getValue();

    hashCode = hashCode * 8191 + ((isSetV()) ? 131071 : 524287);
    if (isSetV())
      hashCode = hashCode * 8191 + v.hashCode();

    return hashCode;
  }

  @Override
  public int compareTo(SpecValueSerial_KEY other) {
    if (!getClass().equals(other.getClass())) {
      return getClass().getName().compareTo(other.getClass().getName());
    }

    int lastComparison = 0;

    lastComparison = java.lang.Boolean.compare(isSetSeq(), other.isSetSeq());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetSeq()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.seq, other.seq);
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
    return 0;
  }

  @org.apache.thrift.annotation.Nullable
  public _Fields fieldForId(int fieldId) {
    return _Fields.findByThriftId(fieldId);
  }

  public void read(org.apache.thrift.protocol.TProtocol iprot) throws org.apache.thrift.TException {
    scheme(iprot).read(iprot, this);
  }

  public void write(org.apache.thrift.protocol.TProtocol oprot) throws org.apache.thrift.TException {
    scheme(oprot).write(oprot, this);
  }

  @Override
  public java.lang.String toString() {
    java.lang.StringBuilder sb = new java.lang.StringBuilder("SpecValueSerial_KEY(");
    boolean first = true;

    sb.append("seq:");
    if (this.seq == null) {
      sb.append("null");
    } else {
      sb.append(this.seq);
    }
    first = false;
    if (!first) sb.append(", ");
    sb.append("v:");
    if (this.v == null) {
      sb.append("null");
    } else {
      sb.append(this.v);
    }
    first = false;
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
      read(new org.apache.thrift.protocol.TCompactProtocol(new org.apache.thrift.transport.TIOStreamTransport(in)));
    } catch (org.apache.thrift.TException te) {
      throw new java.io.IOException(te);
    }
  }

  private static class SpecValueSerial_KEYStandardSchemeFactory implements org.apache.thrift.scheme.SchemeFactory {
    public SpecValueSerial_KEYStandardScheme getScheme() {
      return new SpecValueSerial_KEYStandardScheme();
    }
  }

  private static class SpecValueSerial_KEYStandardScheme extends org.apache.thrift.scheme.StandardScheme<SpecValueSerial_KEY> {

    public void read(org.apache.thrift.protocol.TProtocol iprot, SpecValueSerial_KEY struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TField schemeField;
      iprot.readStructBegin();
      while (true)
      {
        schemeField = iprot.readFieldBegin();
        if (schemeField.type == org.apache.thrift.protocol.TType.STOP) { 
          break;
        }
        switch (schemeField.id) {
          case 1: // SEQ
            if (schemeField.type == org.apache.thrift.protocol.TType.I32) {
              struct.seq = org.swcdb.thrift.gen.KeySeq.findByValue(iprot.readI32());
              struct.setSeqIsSet(true);
            } else { 
              org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
            }
            break;
          case 2: // V
            if (schemeField.type == org.apache.thrift.protocol.TType.LIST) {
              {
                org.apache.thrift.protocol.TList _list104 = iprot.readListBegin();
                struct.v = new java.util.ArrayList<SpecFraction>(_list104.size);
                @org.apache.thrift.annotation.Nullable SpecFraction _elem105;
                for (int _i106 = 0; _i106 < _list104.size; ++_i106)
                {
                  _elem105 = new SpecFraction();
                  _elem105.read(iprot);
                  struct.v.add(_elem105);
                }
                iprot.readListEnd();
              }
              struct.setVIsSet(true);
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

    public void write(org.apache.thrift.protocol.TProtocol oprot, SpecValueSerial_KEY struct) throws org.apache.thrift.TException {
      struct.validate();

      oprot.writeStructBegin(STRUCT_DESC);
      if (struct.seq != null) {
        oprot.writeFieldBegin(SEQ_FIELD_DESC);
        oprot.writeI32(struct.seq.getValue());
        oprot.writeFieldEnd();
      }
      if (struct.v != null) {
        oprot.writeFieldBegin(V_FIELD_DESC);
        {
          oprot.writeListBegin(new org.apache.thrift.protocol.TList(org.apache.thrift.protocol.TType.STRUCT, struct.v.size()));
          for (SpecFraction _iter107 : struct.v)
          {
            _iter107.write(oprot);
          }
          oprot.writeListEnd();
        }
        oprot.writeFieldEnd();
      }
      oprot.writeFieldStop();
      oprot.writeStructEnd();
    }

  }

  private static class SpecValueSerial_KEYTupleSchemeFactory implements org.apache.thrift.scheme.SchemeFactory {
    public SpecValueSerial_KEYTupleScheme getScheme() {
      return new SpecValueSerial_KEYTupleScheme();
    }
  }

  private static class SpecValueSerial_KEYTupleScheme extends org.apache.thrift.scheme.TupleScheme<SpecValueSerial_KEY> {

    @Override
    public void write(org.apache.thrift.protocol.TProtocol prot, SpecValueSerial_KEY struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TTupleProtocol oprot = (org.apache.thrift.protocol.TTupleProtocol) prot;
      java.util.BitSet optionals = new java.util.BitSet();
      if (struct.isSetSeq()) {
        optionals.set(0);
      }
      if (struct.isSetV()) {
        optionals.set(1);
      }
      oprot.writeBitSet(optionals, 2);
      if (struct.isSetSeq()) {
        oprot.writeI32(struct.seq.getValue());
      }
      if (struct.isSetV()) {
        {
          oprot.writeI32(struct.v.size());
          for (SpecFraction _iter108 : struct.v)
          {
            _iter108.write(oprot);
          }
        }
      }
    }

    @Override
    public void read(org.apache.thrift.protocol.TProtocol prot, SpecValueSerial_KEY struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TTupleProtocol iprot = (org.apache.thrift.protocol.TTupleProtocol) prot;
      java.util.BitSet incoming = iprot.readBitSet(2);
      if (incoming.get(0)) {
        struct.seq = org.swcdb.thrift.gen.KeySeq.findByValue(iprot.readI32());
        struct.setSeqIsSet(true);
      }
      if (incoming.get(1)) {
        {
          org.apache.thrift.protocol.TList _list109 = iprot.readListBegin(org.apache.thrift.protocol.TType.STRUCT);
          struct.v = new java.util.ArrayList<SpecFraction>(_list109.size);
          @org.apache.thrift.annotation.Nullable SpecFraction _elem110;
          for (int _i111 = 0; _i111 < _list109.size; ++_i111)
          {
            _elem110 = new SpecFraction();
            _elem110.read(iprot);
            struct.v.add(_elem110);
          }
        }
        struct.setVIsSet(true);
      }
    }
  }

  private static <S extends org.apache.thrift.scheme.IScheme> S scheme(org.apache.thrift.protocol.TProtocol proto) {
    return (org.apache.thrift.scheme.StandardScheme.class.equals(proto.getScheme()) ? STANDARD_SCHEME_FACTORY : TUPLE_SCHEME_FACTORY).getScheme();
  }
}

