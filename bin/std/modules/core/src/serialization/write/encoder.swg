#global marked
#global export
#global namespace Serialization
using Swag, Core.Errors

enum SectionKind
{
    Raw
    Unordered
    Ordered
}

interface IEncoder
{
    mtd isTextual()->bool;

    mtd start(result: *ConcatBuffer) throw;
    mtd end() throw;

    mtd beginField(val: Swag.TypedValue) throw;
    mtd endField(val: Swag.TypedValue) throw;
    mtd beginSection(kind: SectionKind) throw;
    mtd endSection() throw;

    mtd write(data: const ^void, type: typeinfo) throw;
    mtd writeBufferU8(data: const ^u8, count: u64) throw;

    mtd writeBool(value: bool);
    mtd writeS8(value: s8);
    mtd writeS16(value: s16);
    mtd writeS32(value: s32);
    mtd writeS64(value: s64);
    mtd writeU8(value: u8);
    mtd writeU16(value: u16);
    mtd writeU32(value: u32);
    mtd writeU64(value: u64);
    mtd writeF32(value: f32);
    mtd writeF64(value: f64);
    mtd writeString(value: string);
}

/*
Serialization encoder.
This is a high level encoder that needs a specific implementation like [[Core.Serialization.Write.TagBin]] or [[Core.Serialization.Write.JSon]].

```swag
var buf:     ConcatBuffer
var encoder: Encoder'Write.TagBin
encoder.writeAll(&buf, myStruct)
File.writeAllBytes(file, buf)
```

Can be serialized:
* All simple types: `bool/u8/u16/u32/u64/s8/s16/s32/s64/rune/f32/f64`
* A type value 'typeinfo', which will be saved as a CRC.
* An interface. Will be stored as a type and a typed value.
* A struct.
* A `Core.String`.
* A `Core.Array` of valid values.
* A static array of valid values.

> NOTE:
> Type 'string' is not supported. Consider using [[Core.String]] instead.

A struct can implement the 'ISerialize' interface in order to have a specific serialization. If not defined, then the struct will be saved field by field.
*/
struct(T) Encoder
{
    serializer:         T
    errorIfUnknown:     bool     // If true, an error will be raised if a type cannot be saved. The field will be ignored if set to false
}

impl Encoder
{
    // Start serialization
    mtd start(result: *ConcatBuffer) throw
    {
        me.serializer.startWrite(result)
        me.serializer.beginRoot()
    }

    // End serialization
    mtd end() throw
    {
        me.serializer.endRoot()
    }

    // Write a given Swag.TypedValue.
    mtd writeTypeValue(typeVal: TypedValue, data: const ^void) throw
    {
        me.serializer.beginElement(typeVal)
        me.writeValue(data, typeVal.pointedType)
        me.serializer.endElement(typeVal)
    }

    // Write a given value with a given type.
    mtd writeValue(data: const ^void, dataType: typeinfo) throw
    {
        var type = Reflection.makeConcreteAlias(dataType)
        type = Reflection.makeConcreteEnum(type)

        me.serializer.beginValue()
        defer #noerr me.serializer.endValue()

        // Native
        if type.kind == .Native
        {
            let typeNative = cast(const *TypeInfoNative) type
            switch typeNative.nativeKind
            {
            case Bool:
                me.serializer.writeNative(dref cast(const *bool) data)
            case U8:
                me.serializer.writeNative(dref cast(const *u8) data)
            case U16:
                me.serializer.writeNative(dref cast(const *u16) data)
            case U32, Rune:
                me.serializer.writeNative(dref cast(const *u32) data)
            case U64:
                me.serializer.writeNative(dref cast(const *u64) data)
            case S8:
                me.serializer.writeNative(dref cast(const *s8) data)
            case S16:
                me.serializer.writeNative(dref cast(const *s16) data)
            case S32:
                me.serializer.writeNative(dref cast(const *s32) data)
            case S64:
                me.serializer.writeNative(dref cast(const *s64) data)
            case F32:
                me.serializer.writeNative(dref cast(const *f32) data)
            case F64:
                me.serializer.writeNative(dref cast(const *f64) data)
            }

            return
        }

        if Reflection.isType(type)
        {
            let typeType = dref cast(const **TypeInfo) data
            me.serializer.writeNative(typeType ? typeType.crc : 0'u32)
            return
        }

        if Reflection.isInterface(type)
        {
            let itf = cast(const *Swag.Interface) data

            me.serializer.beginSection(.Raw)
            defer #noerr me.serializer.endSection()

            var objType, itfType: typeinfo
            if itf.itable
            {
                objType = itf.itable[cast #bit (u64) -1's64]
                itfType = itf.itable[cast #bit (u64) -2's64]
            }

            var typeVal: TypedValue
            typeVal.pointedType = typeinfo
            me.writeTypeValue(typeVal, &objType)

            if objType
            {
                me.writeTypeValue(typeVal, &itfType)
                typeVal.pointedType = objType
                me.writeTypeValue(typeVal, itf.obj)
            }

            return
        }

        if Reflection.isStruct(type)
        {
            if Reflection.isTuple(type)
            {
                if me.errorIfUnknown:
                    throw UnsupportedFormatError{"cannot serialize an anonymous struct"}
                return
            }

            // Core.String
            if type == Core.String
            {
                let strBuf = cast(const *String) data
                me.serializer.writeNative(strBuf.toString())
                return
            }

            // Core.Array
            let valueType = Reflection.getStructArrayType(type)
            if valueType
            {
                let ptrArr  = cast(const *Array) data
                let treated = me.serializer.beginSequence(type, data, ptrArr.count)
                defer me.serializer.endSequence()

                if !treated
                {
                    var bufArr = cast(const ^u8) ptrArr.buffer
                    for i in ptrArr.count
                    {
                        me.writeValue(bufArr, valueType)
                        bufArr += valueType.sizeof
                    }
                }

                return
            }

            // A structure with an implementation of ISerialize
            let itf = @mkinterface(data, type, ISerialize)
            if itf != null
            {
                if itf.write(cast(IEncoder) me):
                    return
            }

            // Start a fields section
            let structType = cast(const *TypeInfoStruct) type
            let treated    = me.serializer.beginStruct(structType, data)
            defer #noerr me.serializer.endStruct()
            if treated:
                return

            // Generic structure save
            foreach field in structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize):
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize):
                    continue

                if !itf or !itf.writeElement(field, cast(const ^u8) data + field.offset, cast(IEncoder) me)
                {
                    me.writeTypeValue(field, cast(const ^u8) data + field.offset)
                }
            }

            return
        }

        // A static array
        if Reflection.isStaticArray(type)
        {
            let typeArr = cast(const *TypeInfoArray) type
            let treated = me.serializer.beginSequence(typeArr.pointedType, data, typeArr.count)
            defer me.serializer.endSequence()
            if treated:
                return

            var bufArr = cast(const ^u8) data
            for typeArr.count
            {
                me.writeValue(bufArr, typeArr.pointedType)
                bufArr += typeArr.pointedType.sizeof
            }

            return
        }

        if me.errorIfUnknown:
            throw UnsupportedFormatError{Errors.mkString(Format.toString("cannot serialize type [[%]]", type.name))}
    }

    // Write a full struct.
    // Will call [[Encoder.start]], [[Encoder.writeValue]] and [[Encoder.end]]
    mtd writeAll(result: *ConcatBuffer, value: any) throw
    {
        Debug.assert(Reflection.isStruct(@kindof(value)))
        start(me, result)
        me.writeValue(@dataof(value), @kindof(value))
        end(me)
    }
}

impl IEncoder for Encoder
{
    mtd impl isTextual()->bool
    {
        return me.serializer.isTextual()
    }

    // Start serialization
    mtd impl start(result: *ConcatBuffer) throw
    {
        me.serializer.startWrite(result)
        me.serializer.beginRoot()
    }

    // End serialization
    mtd impl end() throw
    {
        me.serializer.endRoot()
    }

    mtd impl beginField(val: Swag.TypedValue) throw
    {
        me.serializer.beginElement(val)
        me.serializer.beginValue()
    }

    mtd impl endField(val: Swag.TypedValue) throw
    {
        me.serializer.endValue()
        me.serializer.endElement(val)
    }

    mtd impl beginSection(kind: SectionKind) throw
    {
        me.serializer.beginSection(kind)
    }

    mtd impl endSection() throw
    {
        me.serializer.endSection()
    }

    mtd impl write(data: const ^void, type: typeinfo) throw
    {
        me.writeValue(data, type)
    }

    mtd impl writeBufferU8(data: const ^u8, count: u64) throw
    {
        let treated = me.serializer.beginSequence(u8, data, count)
        defer #noerr me.serializer.endSequence()

        if !treated
        {
            var ptr = data
            for count
            {
                me.serializer.beginValue()
                me.serializer.writeNative(dref ptr)
                me.serializer.endValue()
                ptr += 1
            }
        }
    }

    mtd impl writeBool(value: bool)     = me.serializer.writeNative(value)
    mtd impl writeS8(value: s8)         = me.serializer.writeNative(value)
    mtd impl writeS16(value: s16)       = me.serializer.writeNative(value)
    mtd impl writeS32(value: s32)       = me.serializer.writeNative(value)
    mtd impl writeS64(value: s64)       = me.serializer.writeNative(value)
    mtd impl writeU8(value: u8)         = me.serializer.writeNative(value)
    mtd impl writeU16(value: u16)       = me.serializer.writeNative(value)
    mtd impl writeU32(value: u32)       = me.serializer.writeNative(value)
    mtd impl writeU64(value: u64)       = me.serializer.writeNative(value)
    mtd impl writeF32(value: f32)       = me.serializer.writeNative(value)
    mtd impl writeF64(value: f64)       = me.serializer.writeNative(value)
    mtd impl writeString(value: string) = me.serializer.writeNative(value)
}
