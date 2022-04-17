/**
 * Autogenerated by Thrift Compiler (0.16.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
package org.swcdb.thrift.gen;


public enum UpdateOP implements org.apache.thrift.TEnum {
  /**
   * The operation to Replace
   */
  REPLACE(0),
  /**
   * The operation to Append
   */
  APPEND(1),
  /**
   * The operation to Prepend
   */
  PREPEND(2),
  /**
   * The operation to Insert
   */
  INSERT(4),
  /**
   * The operation is by inner Serial fields defintions
   */
  SERIAL(8);

  private final int value;

  private UpdateOP(int value) {
    this.value = value;
  }

  /**
   * Get the integer value of this enum value, as defined in the Thrift IDL.
   */
  public int getValue() {
    return value;
  }

  /**
   * Find a the enum type by its integer value, as defined in the Thrift IDL.
   * @return null if the value is not found.
   */
  @org.apache.thrift.annotation.Nullable
  public static UpdateOP findByValue(int value) { 
    switch (value) {
      case 0:
        return REPLACE;
      case 1:
        return APPEND;
      case 2:
        return PREPEND;
      case 4:
        return INSERT;
      case 8:
        return SERIAL;
      default:
        return null;
    }
  }
}
