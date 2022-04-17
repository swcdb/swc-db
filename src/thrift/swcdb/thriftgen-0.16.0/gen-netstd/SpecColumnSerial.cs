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
/// The Column Specifications, the Cells-Intervals(SpecInterval/s) specification for a SERIAL Type Column
/// </summary>
public partial class SpecColumnSerial : TBase
{
  private long _cid;
  private List<SpecIntervalSerial> _intervals;

  /// <summary>
  /// The Column ID
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
  /// The Serial Cells Interval in a list-container
  /// </summary>
  public List<SpecIntervalSerial> Intervals
  {
    get
    {
      return _intervals;
    }
    set
    {
      __isset.intervals = true;
      this._intervals = value;
    }
  }


  public Isset __isset;
  public struct Isset
  {
    public bool cid;
    public bool intervals;
  }

  public SpecColumnSerial()
  {
  }

  public SpecColumnSerial DeepCopy()
  {
    var tmp217 = new SpecColumnSerial();
    if(__isset.cid)
    {
      tmp217.Cid = this.Cid;
    }
    tmp217.__isset.cid = this.__isset.cid;
    if((Intervals != null) && __isset.intervals)
    {
      tmp217.Intervals = this.Intervals.DeepCopy();
    }
    tmp217.__isset.intervals = this.__isset.intervals;
    return tmp217;
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
            if (field.Type == TType.List)
            {
              {
                TList _list218 = await iprot.ReadListBeginAsync(cancellationToken);
                Intervals = new List<SpecIntervalSerial>(_list218.Count);
                for(int _i219 = 0; _i219 < _list218.Count; ++_i219)
                {
                  SpecIntervalSerial _elem220;
                  _elem220 = new SpecIntervalSerial();
                  await _elem220.ReadAsync(iprot, cancellationToken);
                  Intervals.Add(_elem220);
                }
                await iprot.ReadListEndAsync(cancellationToken);
              }
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
      var tmp221 = new TStruct("SpecColumnSerial");
      await oprot.WriteStructBeginAsync(tmp221, cancellationToken);
      var tmp222 = new TField();
      if(__isset.cid)
      {
        tmp222.Name = "cid";
        tmp222.Type = TType.I64;
        tmp222.ID = 1;
        await oprot.WriteFieldBeginAsync(tmp222, cancellationToken);
        await oprot.WriteI64Async(Cid, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((Intervals != null) && __isset.intervals)
      {
        tmp222.Name = "intervals";
        tmp222.Type = TType.List;
        tmp222.ID = 2;
        await oprot.WriteFieldBeginAsync(tmp222, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Struct, Intervals.Count), cancellationToken);
          foreach (SpecIntervalSerial _iter223 in Intervals)
          {
            await _iter223.WriteAsync(oprot, cancellationToken);
          }
          await oprot.WriteListEndAsync(cancellationToken);
        }
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
    if (!(that is SpecColumnSerial other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.cid == other.__isset.cid) && ((!__isset.cid) || (global::System.Object.Equals(Cid, other.Cid))))
      && ((__isset.intervals == other.__isset.intervals) && ((!__isset.intervals) || (TCollections.Equals(Intervals, other.Intervals))));
  }

  public override int GetHashCode() {
    int hashcode = 157;
    unchecked {
      if(__isset.cid)
      {
        hashcode = (hashcode * 397) + Cid.GetHashCode();
      }
      if((Intervals != null) && __isset.intervals)
      {
        hashcode = (hashcode * 397) + TCollections.GetHashCode(Intervals);
      }
    }
    return hashcode;
  }

  public override string ToString()
  {
    var tmp224 = new StringBuilder("SpecColumnSerial(");
    int tmp225 = 0;
    if(__isset.cid)
    {
      if(0 < tmp225++) { tmp224.Append(", "); }
      tmp224.Append("Cid: ");
      Cid.ToString(tmp224);
    }
    if((Intervals != null) && __isset.intervals)
    {
      if(0 < tmp225++) { tmp224.Append(", "); }
      tmp224.Append("Intervals: ");
      Intervals.ToString(tmp224);
    }
    tmp224.Append(')');
    return tmp224.ToString();
  }
}

