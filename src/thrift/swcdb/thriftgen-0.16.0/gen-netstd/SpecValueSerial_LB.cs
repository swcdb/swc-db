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
/// The Specifications of LIST_BYTES(LB) Serial Value Field
/// </summary>
public partial class SpecValueSerial_LB : TBase
{
  private Comp _comp;
  private List<SpecValueSerial_BYTES> _v;

  /// <summary>
  /// Logical comparator to Apply
  /// 
  /// <seealso cref="global::.Comp"/>
  /// </summary>
  public Comp Comp
  {
    get
    {
      return _comp;
    }
    set
    {
      __isset.comp = true;
      this._comp = value;
    }
  }

  /// <summary>
  /// The List of Bytes to match against the value field
  /// </summary>
  public List<SpecValueSerial_BYTES> V
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
    public bool comp;
    public bool v;
  }

  public SpecValueSerial_LB()
  {
  }

  public SpecValueSerial_LB DeepCopy()
  {
    var tmp169 = new SpecValueSerial_LB();
    if(__isset.comp)
    {
      tmp169.Comp = this.Comp;
    }
    tmp169.__isset.comp = this.__isset.comp;
    if((V != null) && __isset.v)
    {
      tmp169.V = this.V.DeepCopy();
    }
    tmp169.__isset.v = this.__isset.v;
    return tmp169;
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
              Comp = (Comp)await iprot.ReadI32Async(cancellationToken);
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
                TList _list170 = await iprot.ReadListBeginAsync(cancellationToken);
                V = new List<SpecValueSerial_BYTES>(_list170.Count);
                for(int _i171 = 0; _i171 < _list170.Count; ++_i171)
                {
                  SpecValueSerial_BYTES _elem172;
                  _elem172 = new SpecValueSerial_BYTES();
                  await _elem172.ReadAsync(iprot, cancellationToken);
                  V.Add(_elem172);
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
      var tmp173 = new TStruct("SpecValueSerial_LB");
      await oprot.WriteStructBeginAsync(tmp173, cancellationToken);
      var tmp174 = new TField();
      if(__isset.comp)
      {
        tmp174.Name = "comp";
        tmp174.Type = TType.I32;
        tmp174.ID = 1;
        await oprot.WriteFieldBeginAsync(tmp174, cancellationToken);
        await oprot.WriteI32Async((int)Comp, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((V != null) && __isset.v)
      {
        tmp174.Name = "v";
        tmp174.Type = TType.List;
        tmp174.ID = 2;
        await oprot.WriteFieldBeginAsync(tmp174, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Struct, V.Count), cancellationToken);
          foreach (SpecValueSerial_BYTES _iter175 in V)
          {
            await _iter175.WriteAsync(oprot, cancellationToken);
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
    if (!(that is SpecValueSerial_LB other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.comp == other.__isset.comp) && ((!__isset.comp) || (global::System.Object.Equals(Comp, other.Comp))))
      && ((__isset.v == other.__isset.v) && ((!__isset.v) || (TCollections.Equals(V, other.V))));
  }

  public override int GetHashCode() {
    int hashcode = 157;
    unchecked {
      if(__isset.comp)
      {
        hashcode = (hashcode * 397) + Comp.GetHashCode();
      }
      if((V != null) && __isset.v)
      {
        hashcode = (hashcode * 397) + TCollections.GetHashCode(V);
      }
    }
    return hashcode;
  }

  public override string ToString()
  {
    var tmp176 = new StringBuilder("SpecValueSerial_LB(");
    int tmp177 = 0;
    if(__isset.comp)
    {
      if(0 < tmp177++) { tmp176.Append(", "); }
      tmp176.Append("Comp: ");
      Comp.ToString(tmp176);
    }
    if((V != null) && __isset.v)
    {
      if(0 < tmp177++) { tmp176.Append(", "); }
      tmp176.Append("V: ");
      V.ToString(tmp176);
    }
    tmp176.Append(')');
    return tmp176.ToString();
  }
}

