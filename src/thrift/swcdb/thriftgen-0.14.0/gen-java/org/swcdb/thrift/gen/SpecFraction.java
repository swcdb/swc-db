/**
 * Autogenerated by Thrift Compiler (0.14.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
package org.swcdb.thrift.gen;

@SuppressWarnings({"cast", "rawtypes", "serial", "unchecked", "unused"})
/**
 * The Fraction Specifications
 */
public class SpecFraction implements org.apache.thrift.TBase<SpecFraction, SpecFraction._Fields>, java.io.Serializable, Cloneable, Comparable<SpecFraction> {
  private static final org.apache.thrift.protocol.TStruct STRUCT_DESC = new org.apache.thrift.protocol.TStruct("SpecFraction");

  private static final org.apache.thrift.protocol.TField COMP_FIELD_DESC = new org.apache.thrift.protocol.TField("comp", org.apache.thrift.protocol.TType.I32, (short)1);
  private static final org.apache.thrift.protocol.TField F_FIELD_DESC = new org.apache.thrift.protocol.TField("f", org.apache.thrift.protocol.TType.STRING, (short)2);

  private static final org.apache.thrift.scheme.SchemeFactory STANDARD_SCHEME_FACTORY = new SpecFractionStandardSchemeFactory();
  private static final org.apache.thrift.scheme.SchemeFactory TUPLE_SCHEME_FACTORY = new SpecFractionTupleSchemeFactory();

  /**
   * Logical comparator to Apply
   * 
   * @see Comp
   */
  public @org.apache.thrift.annotation.Nullable Comp comp; // required
  /**
   * The binary(bytes) to match against a fraction of a Cell-Key
   */
  public @org.apache.thrift.annotation.Nullable java.nio.ByteBuffer f; // required

  /** The set of fields this struct contains, along with convenience methods for finding and manipulating them. */
  public enum _Fields implements org.apache.thrift.TFieldIdEnum {
    /**
     * Logical comparator to Apply
     * 
     * @see Comp
     */
    COMP((short)1, "comp"),
    /**
     * The binary(bytes) to match against a fraction of a Cell-Key
     */
    F((short)2, "f");

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
        case 1: // COMP
          return COMP;
        case 2: // F
          return F;
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
    tmpMap.put(_Fields.COMP, new org.apache.thrift.meta_data.FieldMetaData("comp", org.apache.thrift.TFieldRequirementType.DEFAULT, 
        new org.apache.thrift.meta_data.EnumMetaData(org.apache.thrift.protocol.TType.ENUM, Comp.class)));
    tmpMap.put(_Fields.F, new org.apache.thrift.meta_data.FieldMetaData("f", org.apache.thrift.TFieldRequirementType.DEFAULT, 
        new org.apache.thrift.meta_data.FieldValueMetaData(org.apache.thrift.protocol.TType.STRING        , true)));
    metaDataMap = java.util.Collections.unmodifiableMap(tmpMap);
    org.apache.thrift.meta_data.FieldMetaData.addStructMetaDataMap(SpecFraction.class, metaDataMap);
  }

  public SpecFraction() {
  }

  public SpecFraction(
    Comp comp,
    java.nio.ByteBuffer f)
  {
    this();
    this.comp = comp;
    this.f = org.apache.thrift.TBaseHelper.copyBinary(f);
  }

  /**
   * Performs a deep copy on <i>other</i>.
   */
  public SpecFraction(SpecFraction other) {
    if (other.isSetComp()) {
      this.comp = other.comp;
    }
    if (other.isSetF()) {
      this.f = org.apache.thrift.TBaseHelper.copyBinary(other.f);
    }
  }

  public SpecFraction deepCopy() {
    return new SpecFraction(this);
  }

  @Override
  public void clear() {
    this.comp = null;
    this.f = null;
  }

  /**
   * Logical comparator to Apply
   * 
   * @see Comp
   */
  @org.apache.thrift.annotation.Nullable
  public Comp getComp() {
    return this.comp;
  }

  /**
   * Logical comparator to Apply
   * 
   * @see Comp
   */
  public SpecFraction setComp(@org.apache.thrift.annotation.Nullable Comp comp) {
    this.comp = comp;
    return this;
  }

  public void unsetComp() {
    this.comp = null;
  }

  /** Returns true if field comp is set (has been assigned a value) and false otherwise */
  public boolean isSetComp() {
    return this.comp != null;
  }

  public void setCompIsSet(boolean value) {
    if (!value) {
      this.comp = null;
    }
  }

  /**
   * The binary(bytes) to match against a fraction of a Cell-Key
   */
  public byte[] getF() {
    setF(org.apache.thrift.TBaseHelper.rightSize(f));
    return f == null ? null : f.array();
  }

  public java.nio.ByteBuffer bufferForF() {
    return org.apache.thrift.TBaseHelper.copyBinary(f);
  }

  /**
   * The binary(bytes) to match against a fraction of a Cell-Key
   */
  public SpecFraction setF(byte[] f) {
    this.f = f == null ? (java.nio.ByteBuffer)null   : java.nio.ByteBuffer.wrap(f.clone());
    return this;
  }

  public SpecFraction setF(@org.apache.thrift.annotation.Nullable java.nio.ByteBuffer f) {
    this.f = org.apache.thrift.TBaseHelper.copyBinary(f);
    return this;
  }

  public void unsetF() {
    this.f = null;
  }

  /** Returns true if field f is set (has been assigned a value) and false otherwise */
  public boolean isSetF() {
    return this.f != null;
  }

  public void setFIsSet(boolean value) {
    if (!value) {
      this.f = null;
    }
  }

  public void setFieldValue(_Fields field, @org.apache.thrift.annotation.Nullable java.lang.Object value) {
    switch (field) {
    case COMP:
      if (value == null) {
        unsetComp();
      } else {
        setComp((Comp)value);
      }
      break;

    case F:
      if (value == null) {
        unsetF();
      } else {
        if (value instanceof byte[]) {
          setF((byte[])value);
        } else {
          setF((java.nio.ByteBuffer)value);
        }
      }
      break;

    }
  }

  @org.apache.thrift.annotation.Nullable
  public java.lang.Object getFieldValue(_Fields field) {
    switch (field) {
    case COMP:
      return getComp();

    case F:
      return getF();

    }
    throw new java.lang.IllegalStateException();
  }

  /** Returns true if field corresponding to fieldID is set (has been assigned a value) and false otherwise */
  public boolean isSet(_Fields field) {
    if (field == null) {
      throw new java.lang.IllegalArgumentException();
    }

    switch (field) {
    case COMP:
      return isSetComp();
    case F:
      return isSetF();
    }
    throw new java.lang.IllegalStateException();
  }

  @Override
  public boolean equals(java.lang.Object that) {
    if (that instanceof SpecFraction)
      return this.equals((SpecFraction)that);
    return false;
  }

  public boolean equals(SpecFraction that) {
    if (that == null)
      return false;
    if (this == that)
      return true;

    boolean this_present_comp = true && this.isSetComp();
    boolean that_present_comp = true && that.isSetComp();
    if (this_present_comp || that_present_comp) {
      if (!(this_present_comp && that_present_comp))
        return false;
      if (!this.comp.equals(that.comp))
        return false;
    }

    boolean this_present_f = true && this.isSetF();
    boolean that_present_f = true && that.isSetF();
    if (this_present_f || that_present_f) {
      if (!(this_present_f && that_present_f))
        return false;
      if (!this.f.equals(that.f))
        return false;
    }

    return true;
  }

  @Override
  public int hashCode() {
    int hashCode = 1;

    hashCode = hashCode * 8191 + ((isSetComp()) ? 131071 : 524287);
    if (isSetComp())
      hashCode = hashCode * 8191 + comp.getValue();

    hashCode = hashCode * 8191 + ((isSetF()) ? 131071 : 524287);
    if (isSetF())
      hashCode = hashCode * 8191 + f.hashCode();

    return hashCode;
  }

  @Override
  public int compareTo(SpecFraction other) {
    if (!getClass().equals(other.getClass())) {
      return getClass().getName().compareTo(other.getClass().getName());
    }

    int lastComparison = 0;

    lastComparison = java.lang.Boolean.compare(isSetComp(), other.isSetComp());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetComp()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.comp, other.comp);
      if (lastComparison != 0) {
        return lastComparison;
      }
    }
    lastComparison = java.lang.Boolean.compare(isSetF(), other.isSetF());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetF()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.f, other.f);
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
    java.lang.StringBuilder sb = new java.lang.StringBuilder("SpecFraction(");
    boolean first = true;

    sb.append("comp:");
    if (this.comp == null) {
      sb.append("null");
    } else {
      sb.append(this.comp);
    }
    first = false;
    if (!first) sb.append(", ");
    sb.append("f:");
    if (this.f == null) {
      sb.append("null");
    } else {
      org.apache.thrift.TBaseHelper.toString(this.f, sb);
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

  private static class SpecFractionStandardSchemeFactory implements org.apache.thrift.scheme.SchemeFactory {
    public SpecFractionStandardScheme getScheme() {
      return new SpecFractionStandardScheme();
    }
  }

  private static class SpecFractionStandardScheme extends org.apache.thrift.scheme.StandardScheme<SpecFraction> {

    public void read(org.apache.thrift.protocol.TProtocol iprot, SpecFraction struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TField schemeField;
      iprot.readStructBegin();
      while (true)
      {
        schemeField = iprot.readFieldBegin();
        if (schemeField.type == org.apache.thrift.protocol.TType.STOP) { 
          break;
        }
        switch (schemeField.id) {
          case 1: // COMP
            if (schemeField.type == org.apache.thrift.protocol.TType.I32) {
              struct.comp = org.swcdb.thrift.gen.Comp.findByValue(iprot.readI32());
              struct.setCompIsSet(true);
            } else { 
              org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
            }
            break;
          case 2: // F
            if (schemeField.type == org.apache.thrift.protocol.TType.STRING) {
              struct.f = iprot.readBinary();
              struct.setFIsSet(true);
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

    public void write(org.apache.thrift.protocol.TProtocol oprot, SpecFraction struct) throws org.apache.thrift.TException {
      struct.validate();

      oprot.writeStructBegin(STRUCT_DESC);
      if (struct.comp != null) {
        oprot.writeFieldBegin(COMP_FIELD_DESC);
        oprot.writeI32(struct.comp.getValue());
        oprot.writeFieldEnd();
      }
      if (struct.f != null) {
        oprot.writeFieldBegin(F_FIELD_DESC);
        oprot.writeBinary(struct.f);
        oprot.writeFieldEnd();
      }
      oprot.writeFieldStop();
      oprot.writeStructEnd();
    }

  }

  private static class SpecFractionTupleSchemeFactory implements org.apache.thrift.scheme.SchemeFactory {
    public SpecFractionTupleScheme getScheme() {
      return new SpecFractionTupleScheme();
    }
  }

  private static class SpecFractionTupleScheme extends org.apache.thrift.scheme.TupleScheme<SpecFraction> {

    @Override
    public void write(org.apache.thrift.protocol.TProtocol prot, SpecFraction struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TTupleProtocol oprot = (org.apache.thrift.protocol.TTupleProtocol) prot;
      java.util.BitSet optionals = new java.util.BitSet();
      if (struct.isSetComp()) {
        optionals.set(0);
      }
      if (struct.isSetF()) {
        optionals.set(1);
      }
      oprot.writeBitSet(optionals, 2);
      if (struct.isSetComp()) {
        oprot.writeI32(struct.comp.getValue());
      }
      if (struct.isSetF()) {
        oprot.writeBinary(struct.f);
      }
    }

    @Override
    public void read(org.apache.thrift.protocol.TProtocol prot, SpecFraction struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TTupleProtocol iprot = (org.apache.thrift.protocol.TTupleProtocol) prot;
      java.util.BitSet incoming = iprot.readBitSet(2);
      if (incoming.get(0)) {
        struct.comp = org.swcdb.thrift.gen.Comp.findByValue(iprot.readI32());
        struct.setCompIsSet(true);
      }
      if (incoming.get(1)) {
        struct.f = iprot.readBinary();
        struct.setFIsSet(true);
      }
    }
  }

  private static <S extends org.apache.thrift.scheme.IScheme> S scheme(org.apache.thrift.protocol.TProtocol proto) {
    return (org.apache.thrift.scheme.StandardScheme.class.equals(proto.getScheme()) ? STANDARD_SCHEME_FACTORY : TUPLE_SCHEME_FACTORY).getScheme();
  }
}
