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

    mtd beginField(val: Swag.TypeValue) throw;
    mtd endField(val: Swag.TypeValue) throw;
    mtd beginSection(kind: SectionKind) throw;
    mtd endSection() throw;

    mtd write(data: const [*] void, type: typeinfo) throw;
    mtd writeBufferU8(data: const [*] u8, count: u64) throw;

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
        .serializer.startWrite(result)
        .serializer.beginRoot()
    }

    // End serialization
    mtd end() throw
    {
        .serializer.endRoot()
    }

    // Write a given Swag.TypeValue.
    mtd writeTypeValue(typeVal: TypeValue, data: const [*] void) throw
    {
        .serializer.beginElement(typeVal)
        .writeValue(data, typeVal.pointedType)
        .serializer.endElement(typeVal)
    }

    // Write a given value with a given type.
    mtd writeValue(data: const [*] void, dataType: typeinfo) throw
    {
        var type = Reflection.makeConcreteAlias(dataType)
        type = Reflection.makeConcreteEnum(type)

        .serializer.beginValue()
        defer #noerr .serializer.endValue()

        // Native
        if type.kind == .Native
        {
            let typeNative = cast(const *TypeInfoNative) type
            switch typeNative.nativeKind
            {
            case Bool:
                .serializer.writeNative(dref cast(const *bool) data)
            case U8:
                .serializer.writeNative(dref cast(const *u8) data)
            case U16:
                .serializer.writeNative(dref cast(const *u16) data)
            case U32, Rune:
                .serializer.writeNative(dref cast(const *u32) data)
            case U64:
                .serializer.writeNative(dref cast(const *u64) data)
            case S8:
                .serializer.writeNative(dref cast(const *s8) data)
            case S16:
                .serializer.writeNative(dref cast(const *s16) data)
            case S32:
                .serializer.writeNative(dref cast(const *s32) data)
            case S64:
                .serializer.writeNative(dref cast(const *s64) data)
            case F32:
                .serializer.writeNative(dref cast(const *f32) data)
            case F64:
                .serializer.writeNative(dref cast(const *f64) data)
            }

            return
        }

        if Reflection.isType(type)
        {
            let typeType = dref cast(const **TypeInfo) data
            .serializer.writeNative(typeType ? typeType.crc : 0'u32)
            return
        }

        if Reflection.isInterface(type)
        {
            let itf = cast(const *Swag.Interface) data

            .serializer.beginSection(.Raw)
            defer #noerr .serializer.endSection()

            var objType, itfType: typeinfo
            if itf.itable
            {
                objType = itf.itable[cast #bit (u64) -1's64]
                itfType = itf.itable[cast #bit (u64) -2's64]
            }

            var typeVal: TypeValue
            typeVal.pointedType = typeinfo
            .writeTypeValue(typeVal, &objType)

            if objType
            {
                .writeTypeValue(typeVal, &itfType)
                typeVal.pointedType = objType
                .writeTypeValue(typeVal, itf.obj)
            }

            return
        }

        if Reflection.isStruct(type)
        {
            if Reflection.isTuple(type)
            {
                if .errorIfUnknown do
                    throw UnsupportedFormatError{"cannot serialize an anonymous struct"}
                return
            }

            // Core.String
            if type == Core.String
            {
                let strBuf = cast(const *String) data
                .serializer.writeNative(strBuf.toString())
                return
            }

            // Core.Array
            let valueType = Reflection.getStructArrayType(type)
            if valueType
            {
                let ptrArr  = cast(const *Array) data
                let treated = .serializer.beginSequence(type, data, ptrArr.count)
                defer .serializer.endSequence()

                if !treated
                {
                    var bufArr = cast(const [*] u8) ptrArr.buffer
                    for i in ptrArr.count
                    {
                        .writeValue(bufArr, valueType)
                        bufArr += valueType.sizeof
                    }
                }

                return
            }

            // A structure with an implementation of ISerialize
            let itf = @mkinterface(data, type, ISerialize)
            if itf != null
            {
                if itf.write(cast(IEncoder) me) do
                    return
            }

            // Start a fields section
            let structType = cast(const *TypeInfoStruct) type
            let treated    = .serializer.beginStruct(structType, data)
            defer #noerr .serializer.endStruct()
            if treated do
                return

            // Generic structure save
            foreach field in structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize) do
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize) do
                    continue

                if !itf or !itf.writeElement(field, cast(const [*] u8) data + field.offset, cast(IEncoder) me)
                {
                    .writeTypeValue(field, cast(const [*] u8) data + field.offset)
                }
            }

            return
        }

        // A static array
        if Reflection.isStaticArray(type)
        {
            let typeArr = cast(const *TypeInfoArray) type
            let treated = .serializer.beginSequence(typeArr.pointedType, data, typeArr.count)
            defer .serializer.endSequence()
            if treated do
                return

            var bufArr = cast(const [*] u8) data
            for typeArr.count
            {
                .writeValue(bufArr, typeArr.pointedType)
                bufArr += typeArr.pointedType.sizeof
            }

            return
        }

        if .errorIfUnknown do
            throw UnsupportedFormatError{Errors.mkString(Format.toString("cannot serialize type [[%]]", type.name))}
    }

    // Write a full struct.
    // Will call [[Encoder.start]], [[Encoder.writeValue]] and [[Encoder.end]]
    mtd writeAll(result: *ConcatBuffer, value: any) throw
    {
        Debug.assert(Reflection.isStruct(@kindof(value)))
        start(me, result)
        .writeValue(@dataof(value), @kindof(value))
        end(me)
    }
}

impl IEncoder for Encoder
{
    mtd impl isTextual()->bool
    {
        return .serializer.isTextual()
    }

    // Start serialization
    mtd impl start(result: *ConcatBuffer) throw
    {
        .serializer.startWrite(result)
        .serializer.beginRoot()
    }

    // End serialization
    mtd impl end() throw
    {
        .serializer.endRoot()
    }

    mtd impl beginField(val: Swag.TypeValue) throw
    {
        .serializer.beginElement(val)
        .serializer.beginValue()
    }

    mtd impl endField(val: Swag.TypeValue) throw
    {
        .serializer.endValue()
        .serializer.endElement(val)
    }

    mtd impl beginSection(kind: SectionKind) throw
    {
        .serializer.beginSection(kind)
    }

    mtd impl endSection() throw
    {
        .serializer.endSection()
    }

    mtd impl write(data: const [*] void, type: typeinfo) throw
    {
        .writeValue(data, type)
    }

    mtd impl writeBufferU8(data: const [*] u8, count: u64) throw
    {
        let treated = .serializer.beginSequence(u8, data, count)
        defer #noerr .serializer.endSequence()

        if !treated
        {
            var ptr = data
            for count
            {
                .serializer.beginValue()
                .serializer.writeNative(dref ptr)
                .serializer.endValue()
                ptr += 1
            }
        }
    }

    mtd impl writeBool(value: bool)     => .serializer.writeNative(value)
    mtd impl writeS8(value: s8)         => .serializer.writeNative(value)
    mtd impl writeS16(value: s16)       => .serializer.writeNative(value)
    mtd impl writeS32(value: s32)       => .serializer.writeNative(value)
    mtd impl writeS64(value: s64)       => .serializer.writeNative(value)
    mtd impl writeU8(value: u8)         => .serializer.writeNative(value)
    mtd impl writeU16(value: u16)       => .serializer.writeNative(value)
    mtd impl writeU32(value: u32)       => .serializer.writeNative(value)
    mtd impl writeU64(value: u64)       => .serializer.writeNative(value)
    mtd impl writeF32(value: f32)       => .serializer.writeNative(value)
    mtd impl writeF64(value: f64)       => .serializer.writeNative(value)
    mtd impl writeString(value: string) => .serializer.writeNative(value)
}
