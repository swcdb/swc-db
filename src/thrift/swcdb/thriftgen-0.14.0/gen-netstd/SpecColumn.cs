/**
 * Autogenerated by Thrift Compiler (0.14.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
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


#pragma warning disable IDE0079  // remove unnecessary pragmas
#pragma warning disable IDE1006  // parts of the code use IDL spelling


/// <summary>
/// The Column Specifications, the Cells-Intervals(SpecInterval/s) specification for a column
/// </summary>
public partial class SpecColumn : TBase
{
  private long _cid;
  private List<SpecInterval> _intervals;

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
  /// The Cells Interval in a list-container
  /// </summary>
  public List<SpecInterval> Intervals
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

  public SpecColumn()
  {
  }

  public SpecColumn DeepCopy()
  {
    var tmp72 = new SpecColumn();
    if(__isset.cid)
    {
      tmp72.Cid = this.Cid;
    }
    tmp72.__isset.cid = this.__isset.cid;
    if((Intervals != null) && __isset.intervals)
    {
      tmp72.Intervals = this.Intervals.DeepCopy();
    }
    tmp72.__isset.intervals = this.__isset.intervals;
    return tmp72;
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
                TList _list73 = await iprot.ReadListBeginAsync(cancellationToken);
                Intervals = new List<SpecInterval>(_list73.Count);
                for(int _i74 = 0; _i74 < _list73.Count; ++_i74)
                {
                  SpecInterval _elem75;
                  _elem75 = new SpecInterval();
                  await _elem75.ReadAsync(iprot, cancellationToken);
                  Intervals.Add(_elem75);
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
      var struc = new TStruct("SpecColumn");
      await oprot.WriteStructBeginAsync(struc, cancellationToken);
      var field = new TField();
      if(__isset.cid)
      {
        field.Name = "cid";
        field.Type = TType.I64;
        field.ID = 1;
        await oprot.WriteFieldBeginAsync(field, cancellationToken);
        await oprot.WriteI64Async(Cid, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((Intervals != null) && __isset.intervals)
      {
        field.Name = "intervals";
        field.Type = TType.List;
        field.ID = 2;
        await oprot.WriteFieldBeginAsync(field, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Struct, Intervals.Count), cancellationToken);
          foreach (SpecInterval _iter76 in Intervals)
          {
            await _iter76.WriteAsync(oprot, cancellationToken);
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
    if (!(that is SpecColumn other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.cid == other.__isset.cid) && ((!__isset.cid) || (System.Object.Equals(Cid, other.Cid))))
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
    var sb = new StringBuilder("SpecColumn(");
    int tmp77 = 0;
    if(__isset.cid)
    {
      if(0 < tmp77++) { sb.Append(", "); }
      sb.Append("Cid: ");
      Cid.ToString(sb);
    }
    if((Intervals != null) && __isset.intervals)
    {
      if(0 < tmp77++) { sb.Append(", "); }
      sb.Append("Intervals: ");
      Intervals.ToString(sb);
    }
    sb.Append(')');
    return sb.ToString();
  }
}

