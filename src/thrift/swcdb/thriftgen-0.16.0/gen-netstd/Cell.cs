/**
 * <auto-generated>
 * Autogenerated by Thrift Compiler (0.16.0)
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 * </auto-generated>
 */
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using Thrift;
using Thrift.Collections;

using Thrift.Protocol;
using Thrift.Protocol.Entities;
using Thrift.Protocol.Utilities;
using Thrift.Transport;
using Thrift.Transport.Client;
using Thrift.Transport.Server;
using Thrift.Processor;


#nullable disable                // suppress C# 8.0 nullable contexts (we still support earlier versions)
#pragma warning disable IDE0079  // remove unnecessary pragmas
#pragma warning disable IDE1006  // parts of the code use IDL spelling
#pragma warning disable IDE0083  // pattern matching "that is not SomeType" requires net5.0 but we still support earlier versions


/// <summary>
/// The Cell for results list of scan
/// </summary>
public partial class Cell : TBase
{
  private string _c;
  private List<byte[]> _k;
  private long _ts;
  private byte[] _v;

  /// <summary>
  /// The Column Name
  /// </summary>
  public string C
  {
    get
    {
      return _c;
    }
    set
    {
      __isset.c = true;
      this._c = value;
    }
  }

  /// <summary>
  /// The Cell Key
  /// </summary>
  public List<byte[]> K
  {
    get
    {
      return _k;
    }
    set
    {
      __isset.k = true;
      this._k = value;
    }
  }

  /// <summary>
  /// The Cell Timestamp
  /// </summary>
  public long Ts
  {
    get
    {
      return _ts;
    }
    set
    {
      __isset.ts = true;
      this._ts = value;
    }
  }

  /// <summary>
  /// The Cell Value
  /// </summary>
  public byte[] V
  {
    get
    {
      return _v;
    }
    set
    {
      __isset.v = true;
      this._v = value;
    }
  }


  public Isset __isset;
  public struct Isset
  {
    public bool c;
    public bool k;
    public bool ts;
    public bool v;
  }

  public Cell()
  {
  }

  public Cell DeepCopy()
  {
    var tmp278 = new Cell();
    if((C != null) && __isset.c)
    {
      tmp278.C = this.C;
    }
    tmp278.__isset.c = this.__isset.c;
    if((K != null) && __isset.k)
    {
      tmp278.K = this.K.DeepCopy();
    }
    tmp278.__isset.k = this.__isset.k;
    if(__isset.ts)
    {
      tmp278.Ts = this.Ts;
    }
    tmp278.__isset.ts = this.__isset.ts;
    if((V != null) && __isset.v)
    {
      tmp278.V = this.V.ToArray();
    }
    tmp278.__isset.v = this.__isset.v;
    return tmp278;
  }

  public async global::System.Threading.Tasks.Task ReadAsync(TProtocol iprot, CancellationToken cancellationToken)
  {
    iprot.IncrementRecursionDepth();
    try
    {
      TField field;
      await iprot.ReadStructBeginAsync(cancellationToken);
      while (true)
      {
        field = await iprot.ReadFieldBeginAsync(cancellationToken);
        if (field.Type == TType.Stop)
        {
          break;
        }

        switch (field.ID)
        {
          case 1:
            if (field.Type == TType.String)
            {
              C = await iprot.ReadStringAsync(cancellationToken);
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          case 2:
            if (field.Type == TType.List)
            {
              {
                TList _list279 = await iprot.ReadListBeginAsync(cancellationToken);
                K = new List<byte[]>(_list279.Count);
                for(int _i280 = 0; _i280 < _list279.Count; ++_i280)
                {
                  byte[] _elem281;
                  _elem281 = await iprot.ReadBinaryAsync(cancellationToken);
                  K.Add(_elem281);
                }
                await iprot.ReadListEndAsync(cancellationToken);
              }
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          case 3:
            if (field.Type == TType.I64)
            {
              Ts = await iprot.ReadI64Async(cancellationToken);
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          case 4:
            if (field.Type == TType.String)
            {
              V = await iprot.ReadBinaryAsync(cancellationToken);
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          default: 
            await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            break;
        }

        await iprot.ReadFieldEndAsync(cancellationToken);
      }

      await iprot.ReadStructEndAsync(cancellationToken);
    }
    finally
    {
      iprot.DecrementRecursionDepth();
    }
  }

  public async global::System.Threading.Tasks.Task WriteAsync(TProtocol oprot, CancellationToken cancellationToken)
  {
    oprot.IncrementRecursionDepth();
    try
    {
      var tmp282 = new TStruct("Cell");
      await oprot.WriteStructBeginAsync(tmp282, cancellationToken);
      var tmp283 = new TField();
      if((C != null) && __isset.c)
      {
        tmp283.Name = "c";
        tmp283.Type = TType.String;
        tmp283.ID = 1;
        await oprot.WriteFieldBeginAsync(tmp283, cancellationToken);
        await oprot.WriteStringAsync(C, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((K != null) && __isset.k)
      {
        tmp283.Name = "k";
        tmp283.Type = TType.List;
        tmp283.ID = 2;
        await oprot.WriteFieldBeginAsync(tmp283, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.String, K.Count), cancellationToken);
          foreach (byte[] _iter284 in K)
          {
            await oprot.WriteBinaryAsync(_iter284, cancellationToken);
          }
          await oprot.WriteListEndAsync(cancellationToken);
        }
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if(__isset.ts)
      {
        tmp283.Name = "ts";
        tmp283.Type = TType.I64;
        tmp283.ID = 3;
        await oprot.WriteFieldBeginAsync(tmp283, cancellationToken);
        await oprot.WriteI64Async(Ts, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((V != null) && __isset.v)
      {
        tmp283.Name = "v";
        tmp283.Type = TType.String;
        tmp283.ID = 4;
        await oprot.WriteFieldBeginAsync(tmp283, cancellationToken);
        await oprot.WriteBinaryAsync(V, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      await oprot.WriteFieldStopAsync(cancellationToken);
      await oprot.WriteStructEndAsync(cancellationToken);
    }
    finally
    {
      oprot.DecrementRecursionDepth();
    }
  }

  public override bool Equals(object that)
  {
    if (!(that is Cell other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.c == other.__isset.c) && ((!__isset.c) || (global::System.Object.Equals(C, other.C))))
      && ((__isset.k == other.__isset.k) && ((!__isset.k) || (global::System.Object.Equals(K, other.K))))
      && ((__isset.ts == other.__isset.ts) && ((!__isset.ts) || (global::System.Object.Equals(Ts, other.Ts))))
      && ((__isset.v == other.__isset.v) && ((!__isset.v) || (TCollections.Equals(V, other.V))));
  }

  public override int GetHashCode() {
    int hashcode = 157;
    unchecked {
      if((C != null) && __isset.c)
      {
        hashcode = (hashcode * 397) + C.GetHashCode();
      }
      if((K != null) && __isset.k)
      {
        hashcode = (hashcode * 397) + K.GetHashCode();
      }
      if(__isset.ts)
      {
        hashcode = (hashcode * 397) + Ts.GetHashCode();
      }
      if((V != null) && __isset.v)
      {
        hashcode = (hashcode * 397) + V.GetHashCode();
      }
    }
    return hashcode;
  }

  public override string ToString()
  {
    var tmp285 = new StringBuilder("Cell(");
    int tmp286 = 0;
    if((C != null) && __isset.c)
    {
      if(0 < tmp286++) { tmp285.Append(", "); }
      tmp285.Append("C: ");
      C.ToString(tmp285);
    }
    if((K != null) && __isset.k)
    {
      if(0 < tmp286++) { tmp285.Append(", "); }
      tmp285.Append("K: ");
      K.ToString(tmp285);
    }
    if(__isset.ts)
    {
      if(0 < tmp286++) { tmp285.Append(", "); }
      tmp285.Append("Ts: ");
      Ts.ToString(tmp285);
    }
    if((V != null) && __isset.v)
    {
      if(0 < tmp286++) { tmp285.Append(", "); }
      tmp285.Append("V: ");
      V.ToString(tmp285);
    }
    tmp285.Append(')');
    return tmp285.ToString();
  }
}

