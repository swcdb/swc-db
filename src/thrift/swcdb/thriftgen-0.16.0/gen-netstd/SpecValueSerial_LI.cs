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
/// The Specifications of LIST_INT64(LI) Serial Value Field
/// </summary>
public partial class SpecValueSerial_LI : TBase
{
  private Comp _comp;
  private List<SpecValueSerial_INT64> _v;

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
  /// The List of Int64 to match against the value field
  /// </summary>
  public List<SpecValueSerial_INT64> V
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

  public SpecValueSerial_LI()
  {
  }

  public SpecValueSerial_LI DeepCopy()
  {
    var tmp160 = new SpecValueSerial_LI();
    if(__isset.comp)
    {
      tmp160.Comp = this.Comp;
    }
    tmp160.__isset.comp = this.__isset.comp;
    if((V != null) && __isset.v)
    {
      tmp160.V = this.V.DeepCopy();
    }
    tmp160.__isset.v = this.__isset.v;
    return tmp160;
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
                TList _list161 = await iprot.ReadListBeginAsync(cancellationToken);
                V = new List<SpecValueSerial_INT64>(_list161.Count);
                for(int _i162 = 0; _i162 < _list161.Count; ++_i162)
                {
                  SpecValueSerial_INT64 _elem163;
                  _elem163 = new SpecValueSerial_INT64();
                  await _elem163.ReadAsync(iprot, cancellationToken);
                  V.Add(_elem163);
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
      var tmp164 = new TStruct("SpecValueSerial_LI");
      await oprot.WriteStructBeginAsync(tmp164, cancellationToken);
      var tmp165 = new TField();
      if(__isset.comp)
      {
        tmp165.Name = "comp";
        tmp165.Type = TType.I32;
        tmp165.ID = 1;
        await oprot.WriteFieldBeginAsync(tmp165, cancellationToken);
        await oprot.WriteI32Async((int)Comp, cancellationToken);
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((V != null) && __isset.v)
      {
        tmp165.Name = "v";
        tmp165.Type = TType.List;
        tmp165.ID = 2;
        await oprot.WriteFieldBeginAsync(tmp165, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Struct, V.Count), cancellationToken);
          foreach (SpecValueSerial_INT64 _iter166 in V)
          {
            await _iter166.WriteAsync(oprot, cancellationToken);
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
    if (!(that is SpecValueSerial_LI other)) return false;
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
    var tmp167 = new StringBuilder("SpecValueSerial_LI(");
    int tmp168 = 0;
    if(__isset.comp)
    {
      if(0 < tmp168++) { tmp167.Append(", "); }
      tmp167.Append("Comp: ");
      Comp.ToString(tmp167);
    }
    if((V != null) && __isset.v)
    {
      if(0 < tmp168++) { tmp167.Append(", "); }
      tmp167.Append("V: ");
      V.ToString(tmp167);
    }
    tmp167.Append(')');
    return tmp167.ToString();
  }
}

