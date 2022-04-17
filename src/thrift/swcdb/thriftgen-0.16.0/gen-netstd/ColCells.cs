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
/// The Column Cells for results on Columns of scan
/// </summary>
public partial class ColCells : TBase
{
  private List<CCell> _cells;
  private List<CCellSerial> _serial_cells;

  /// <summary>
  /// The Cells, defined as CCell items in a list-container
  /// </summary>
  public List<CCell> Cells
  {
    get
    {
      return _cells;
    }
    set
    {
      __isset.cells = true;
      this._cells = value;
    }
  }

  /// <summary>
  /// The Serial Cells, defined as CCellSerial items in a list-container
  /// </summary>
  public List<CCellSerial> Serial_cells
  {
    get
    {
      return _serial_cells;
    }
    set
    {
      __isset.serial_cells = true;
      this._serial_cells = value;
    }
  }


  public Isset __isset;
  public struct Isset
  {
    public bool cells;
    public bool serial_cells;
  }

  public ColCells()
  {
  }

  public ColCells DeepCopy()
  {
    var tmp335 = new ColCells();
    if((Cells != null) && __isset.cells)
    {
      tmp335.Cells = this.Cells.DeepCopy();
    }
    tmp335.__isset.cells = this.__isset.cells;
    if((Serial_cells != null) && __isset.serial_cells)
    {
      tmp335.Serial_cells = this.Serial_cells.DeepCopy();
    }
    tmp335.__isset.serial_cells = this.__isset.serial_cells;
    return tmp335;
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
            if (field.Type == TType.List)
            {
              {
                TList _list336 = await iprot.ReadListBeginAsync(cancellationToken);
                Cells = new List<CCell>(_list336.Count);
                for(int _i337 = 0; _i337 < _list336.Count; ++_i337)
                {
                  CCell _elem338;
                  _elem338 = new CCell();
                  await _elem338.ReadAsync(iprot, cancellationToken);
                  Cells.Add(_elem338);
                }
                await iprot.ReadListEndAsync(cancellationToken);
              }
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
                TList _list339 = await iprot.ReadListBeginAsync(cancellationToken);
                Serial_cells = new List<CCellSerial>(_list339.Count);
                for(int _i340 = 0; _i340 < _list339.Count; ++_i340)
                {
                  CCellSerial _elem341;
                  _elem341 = new CCellSerial();
                  await _elem341.ReadAsync(iprot, cancellationToken);
                  Serial_cells.Add(_elem341);
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
      var tmp342 = new TStruct("ColCells");
      await oprot.WriteStructBeginAsync(tmp342, cancellationToken);
      var tmp343 = new TField();
      if((Cells != null) && __isset.cells)
      {
        tmp343.Name = "cells";
        tmp343.Type = TType.List;
        tmp343.ID = 1;
        await oprot.WriteFieldBeginAsync(tmp343, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Struct, Cells.Count), cancellationToken);
          foreach (CCell _iter344 in Cells)
          {
            await _iter344.WriteAsync(oprot, cancellationToken);
          }
          await oprot.WriteListEndAsync(cancellationToken);
        }
        await oprot.WriteFieldEndAsync(cancellationToken);
      }
      if((Serial_cells != null) && __isset.serial_cells)
      {
        tmp343.Name = "serial_cells";
        tmp343.Type = TType.List;
        tmp343.ID = 2;
        await oprot.WriteFieldBeginAsync(tmp343, cancellationToken);
        {
          await oprot.WriteListBeginAsync(new TList(TType.Struct, Serial_cells.Count), cancellationToken);
          foreach (CCellSerial _iter345 in Serial_cells)
          {
            await _iter345.WriteAsync(oprot, cancellationToken);
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
    if (!(that is ColCells other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return ((__isset.cells == other.__isset.cells) && ((!__isset.cells) || (TCollections.Equals(Cells, other.Cells))))
      && ((__isset.serial_cells == other.__isset.serial_cells) && ((!__isset.serial_cells) || (TCollections.Equals(Serial_cells, other.Serial_cells))));
  }

  public override int GetHashCode() {
    int hashcode = 157;
    unchecked {
      if((Cells != null) && __isset.cells)
      {
        hashcode = (hashcode * 397) + TCollections.GetHashCode(Cells);
      }
      if((Serial_cells != null) && __isset.serial_cells)
      {
        hashcode = (hashcode * 397) + TCollections.GetHashCode(Serial_cells);
      }
    }
    return hashcode;
  }

  public override string ToString()
  {
    var tmp346 = new StringBuilder("ColCells(");
    int tmp347 = 0;
    if((Cells != null) && __isset.cells)
    {
      if(0 < tmp347++) { tmp346.Append(", "); }
      tmp346.Append("Cells: ");
      Cells.ToString(tmp346);
    }
    if((Serial_cells != null) && __isset.serial_cells)
    {
      if(0 < tmp347++) { tmp346.Append(", "); }
      tmp346.Append("Serial_cells: ");
      Serial_cells.ToString(tmp346);
    }
    tmp346.Append(')');
    return tmp346.ToString();
  }
}

