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
/// The Compact Result
/// </summary>
public partial class CompactResult : TBase
{
  private long _cid;
  private int _err;

  /// <summary>
  /// Column ID
  /// </summary>
  public long Cid
  {
    get
    {
      return _cid;
    }
    set
    {
      __isset.cid = true;
      this._cid = value;
    }
  }

  /// <summary>
  /// Error
  /// </summary>
  public int Err
  {
    get
    {
      return _err;
    }
    set
    {
      __isset.err = true;
      this._err = value;
    }
  }


  public Isset __isset;
  public struct Isset
  {
    public bool cid;
    public bool err;
  }

  public CompactResult()
  {
  }

  public CompactResult DeepCopy()
  {
    var tmp425 = new CompactResult();
    if(__isset.cid)
    {
      tmp425.Cid = this.Cid;
    }
    tmp425.__isset.cid = this.__isset.cid;
    if(__isset.err)
    {
      tmp425.Err = this.Err;
    }
    tmp425.__isset.err = this.__isset.err;
    return tmp425;
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
            if (field.Type == TType.I64)
            {
              Cid = await iprot.ReadI64Async(cancellationToken);
            }
            else
            {
              await TProtocolUtil.SkipAsync(iprot, field.Type, cancellationToken);
            }
            break;
          case 2:
            if (field.Type == TType.I32)
            {
              Err = await iprot.ReadI32Async(cancellationToken);
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
      var tmp426 = new TStruct("CompactResult");
      await oprot.WriteStructBeginAsync(tmp426, cancellationToken);
      var tmp427 = new TField();
      if(__isset.cid)
      {
        tmp427.Name = "cid";
        tmp427.Type = TType.I64;
        tmp427.ID = 1;
        await oprot.WriteFieldBeginAsync(tmp427, cancellationToken);
        await oprot.WriteI64Async(Cid, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if(__isset.err)
      {
        tmp427.Name = "err";
        tmp427.Type = TType.I32;
        tmp427.ID = 2;
        await oprot.WriteFieldBeginAsync(tmp427, cancellationToken);
        await oprot.WriteI32Async(Err, cancellationToken);
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
    if (!(that is CompactResult other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.cid == other.__isset.cid) && ((!__isset.cid) || (global::System.Object.Equals(Cid, other.Cid))))
      && ((__isset.err == other.__isset.err) && ((!__isset.err) || (global::System.Object.Equals(Err, other.Err))));
  }

  public override int GetHashCode() {
    int hashcode = 157;
    unchecked {
      if(__isset.cid)
      {
        hashcode = (hashcode * 397) + Cid.GetHashCode();
      }
      if(__isset.err)
      {
        hashcode = (hashcode * 397) + Err.GetHashCode();
      }
    }
    return hashcode;
  }

  public override string ToString()
  {
    var tmp428 = new StringBuilder("CompactResult(");
    int tmp429 = 0;
    if(__isset.cid)
    {
      if(0 < tmp429++) { tmp428.Append(", "); }
      tmp428.Append("Cid: ");
      Cid.ToString(tmp428);
    }
    if(__isset.err)
    {
      if(0 < tmp429++) { tmp428.Append(", "); }
      tmp428.Append("Err: ");
      Err.ToString(tmp428);
    }
    tmp428.Append(')');
    return tmp428.ToString();
  }
}

