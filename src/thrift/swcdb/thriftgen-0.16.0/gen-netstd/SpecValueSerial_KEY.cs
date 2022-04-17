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
/// The Specifications of KEY Serial Value Field
/// </summary>
public partial class SpecValueSerial_KEY : TBase
{
  private KeySeq _seq;
  private List<SpecFraction> _v;

  /// <summary>
  /// The Key Sequence to use
  /// 
  /// <seealso cref="global::.KeySeq"/>
  /// </summary>
  public KeySeq Seq
  {
    get
    {
      return _seq;
    }
    set
    {
      __isset.seq = true;
      this._seq = value;
    }
  }

  /// <summary>
  /// The Specification of the Key to match against the value field
  /// </summary>
  public List<SpecFraction> V
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
    public bool seq;
    public bool v;
  }

  public SpecValueSerial_KEY()
  {
  }

  public SpecValueSerial_KEY DeepCopy()
  {
    var tmp151 = new SpecValueSerial_KEY();
    if(__isset.seq)
    {
      tmp151.Seq = this.Seq;
    }
    tmp151.__isset.seq = this.__isset.seq;
    if((V != null) && __isset.v)
    {
      tmp151.V = this.V.DeepCopy();
    }
    tmp151.__isset.v = this.__isset.v;
    return tmp151;
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
            if (field.Type == TType.I32)
            {
              Seq = (KeySeq)await iprot.ReadI32Async(cancellationToken);
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
                TList _list152 = await iprot.ReadListBeginAsync(cancellationToken);
                V = new List<SpecFraction>(_list152.Count);
                for(int _i153 = 0; _i153 < _list152.Count; ++_i153)
                {
                  SpecFraction _elem154;
                  _elem154 = new SpecFraction();
                  await _elem154.ReadAsync(iprot, cancellationToken);
                  V.Add(_elem154);
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
      var tmp155 = new TStruct("SpecValueSerial_KEY");
      await oprot.WriteStructBeginAsync(tmp155, cancellationToken);
      var tmp156 = new TField();
      if(__isset.seq)
      {
        tmp156.Name = "seq";
        tmp156.Type = TType.I32;
        tmp156.ID = 1;
        await oprot.WriteFieldBeginAsync(tmp156, cancellationToken);
        await oprot.WriteI32Async((int)Seq, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((V != null) && __isset.v)
      {
        tmp156.Name = "v";
        tmp156.Type = TType.List;
        tmp156.ID = 2;
        await oprot.WriteFieldBeginAsync(tmp156, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Struct, V.Count), cancellationToken);
          foreach (SpecFraction _iter157 in V)
          {
            await _iter157.WriteAsync(oprot, cancellationToken);
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
    if (!(that is SpecValueSerial_KEY other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.seq == other.__isset.seq) && ((!__isset.seq) || (global::System.Object.Equals(Seq, other.Seq))))
      && ((__isset.v == other.__isset.v) && ((!__isset.v) || (global::System.Object.Equals(V, other.V))));
  }

  public override int GetHashCode() {
    int hashcode = 157;
    unchecked {
      if(__isset.seq)
      {
        hashcode = (hashcode * 397) + Seq.GetHashCode();
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
    var tmp158 = new StringBuilder("SpecValueSerial_KEY(");
    int tmp159 = 0;
    if(__isset.seq)
    {
      if(0 < tmp159++) { tmp158.Append(", "); }
      tmp158.Append("Seq: ");
      Seq.ToString(tmp158);
    }
    if((V != null) && __isset.v)
    {
      if(0 < tmp159++) { tmp158.Append(", "); }
      tmp158.Append("V: ");
      V.ToString(tmp158);
    }
    tmp158.Append(')');
    return tmp158.ToString();
  }
}

