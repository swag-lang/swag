#global export
#global namespace Serialization
using Swag

enum SectionKind
{
    Raw
    Unordered
    Ordered
}

interface IEncoder
{
    mtd start(result: *ConcatBuffer) throw
    mtd end() throw

    mtd beginField(val: Swag.TypeValue) throw
    mtd endField() throw
    mtd beginSection(kind: SectionKind) throw
    mtd endSection() throw

    mtd write(data: const ^void, type: typeinfo) throw
    mtd writeBufferU8(data: const ^u8, count: u64) throw

    mtd writeBool(value: bool)
    mtd writeS8(value: s8)
    mtd writeS16(value: s16)
    mtd writeS32(value: s32)
    mtd writeS64(value: s64)
    mtd writeU8(value: u8)
    mtd writeU16(value: u16)
    mtd writeU32(value: u32)
    mtd writeU64(value: u64)
    mtd writeF32(value: f32)
    mtd writeF64(value: f64)
}

struct(T) Encoder
{
    serializer: T
    errorIfUnknown: bool // If true, raise an error if a type cannot be saved, and just ignore if false
}

impl Encoder
{
    // Start serialization
    mtd start(result: *ConcatBuffer) throw
    {
        serializer.startWrite(result)
        serializer.beginRoot()
    }

    // End serialization
    mtd end() throw
    {
        serializer.endRoot()
    }

    mtd writeTypeValue(typeVal: TypeValue, data: const ^void) throw
    {
        serializer.beginElement(typeVal)
        writeValue(data, typeVal.pointedType)
        serializer.endElement()
    }

    mtd writeValue(data: const ^void, dataType: typeinfo) throw
    {
        var type = Reflection.makeConcreteAlias(dataType)
        type = Reflection.makeConcreteEnum(type)

        serializer.beginValue()
        defer(noerr) serializer.endValue()

        // Native
        if type.kind == .Native
        {
            var typeNative = cast(const *TypeInfoNative) type
            switch typeNative.nativeKind
            {
            case Bool:
                serializer.writeNative(dref cast(const *bool) data)
            case U8:
                serializer.writeNative(dref cast(const *u8) data)
            case U16:
                serializer.writeNative(dref cast(const *u16) data)
            case U32, Rune:
                serializer.writeNative(dref cast(const *u32) data)
            case U64:
                serializer.writeNative(dref cast(const *u64) data)
            case S8:
                serializer.writeNative(dref cast(const *s8) data)
            case S16:
                serializer.writeNative(dref cast(const *s16) data)
            case S32:
                serializer.writeNative(dref cast(const *s32) data)
            case S64:
                serializer.writeNative(dref cast(const *s64) data)
            case F32:
                serializer.writeNative(dref cast(const *f32) data)
            case F64:
                serializer.writeNative(dref cast(const *f64) data)
            }

            return
        }

        if Reflection.isType(type)
        {
            var typeType = dref cast(const **TypeInfo) data
            serializer.writeNative(typeType ? typeType.crc32 : 0'u32)
            return
        }

        if Reflection.isInterface(type)
        {
            var itf = cast(const *Swag.Interface) data

            serializer.beginSection(.Raw)
            defer serializer.endSection()

            var objType, itfType: typeinfo
            if itf.itable
            {
                objType = itf.itable[cast,bit(u64) -1's64]
                itfType = itf.itable[cast,bit(u64) -2's64]
            }

            var typeVal: TypeValue
            typeVal.pointedType = typeinfo
            writeTypeValue(typeVal, &objType)

            if objType
            {
                writeTypeValue(typeVal, &itfType)
                typeVal.pointedType = objType
                writeTypeValue(typeVal, itf.obj)
            }

            return
        }

        if Reflection.isStruct(type)
        {
            if Reflection.isTuple(type)
            {
                if errorIfUnknown
                    throw "cannot serialize an anonymous struct"
                return
            }

            // Core.String
            if type == String
            {
                var strBuf = cast(const *String) data
                serializer.writeNative(strBuf.toString())
                return
            }

            var structType = cast(const *TypeInfoStruct) type

            // Core.Array
            if structType.fromGeneric == @typeof(Array)
            {
                var valueType = structType.generics[0].pointedType
                var ptrArr = cast(const *Array) data

                var treated = serializer.beginSequence(type, data, ptrArr.count)
                defer serializer.endSequence()

                if !treated
                {
                    var bufArr = cast(const ^u8) ptrArr.buffer
                    loop i: ptrArr.count
                    {
                        writeValue(bufArr, valueType)
                        bufArr += valueType.sizeof
                    }
                }

                return
            }

            // A structure with an implementation of ISerialize
            var itf = @mkinterface(data, type, ISerialize)
            if itf != null
            {
                if itf.write(cast(IEncoder) self)
                    return
            }

            // Start a fields section
            var treated = serializer.beginStruct(structType, data)
            defer(noerr) serializer.endStruct()
            if treated
                return

            // Generic structure save
            visit field: structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize)
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize)
                    continue

                if !itf or !itf.writeElement(field, cast(const ^u8) data + field.offset, cast(IEncoder) self)
                {
                    writeTypeValue(field, cast(const ^u8) data + field.offset)
                }
            }

            return
        }

        // A static array
        if Reflection.isArray(type)
        {
            var typeArr = cast(const *TypeInfoArray) type
            var treated = serializer.beginSequence(typeArr.pointedType, data, typeArr.count)
            defer serializer.endSequence()
            if treated
                return

            var bufArr = cast(const ^u8) data
            loop typeArr.count
            {
                writeValue(bufArr, typeArr.pointedType)
                bufArr += typeArr.pointedType.sizeof
            }

            return
        }

        if errorIfUnknown
            throw Format.toString("cannot serialize type '%'", type.name)
    }

    // Write a full struct
    mtd writeAll(result: *ConcatBuffer, value: any) throw
    {
        Debug.assert(Reflection.isStruct(@kindof(value)))
        start(self, result)
        writeValue(@dataof(value), @kindof(value))
        end(self)
    }
}

impl IEncoder for Encoder
{
    // Start serialization
    mtd impl start(result: *ConcatBuffer) throw
    {
        serializer.startWrite(result)
        serializer.beginRoot()
    }

    // End serialization
    mtd impl end() throw
    {
        serializer.endRoot()
    }

    mtd impl beginField(val: Swag.TypeValue) throw
    {
        serializer.beginElement(val)
        serializer.beginValue()
    }

    mtd impl endField() throw
    {
        serializer.endValue()
        serializer.endElement()
    }

    mtd impl beginSection(kind: SectionKind) throw
    {
        serializer.beginSection(kind)
    }

    mtd impl endSection() throw
    {
        serializer.endSection()
    }

    mtd impl write(data: const ^void, type: typeinfo) throw
    {
        writeValue(data, type)
    }

    mtd impl writeBufferU8(data: const ^u8, count: u64) throw
    {
        var treated = serializer.beginSequence(u8, data, count)
        defer(noerr) serializer.endSequence()

        if !treated
        {
            var ptr = data
            loop count
            {
                serializer.beginValue()
                serializer.writeNative(dref ptr)
                serializer.endValue()
                ptr += 1
            }
        }
    }

    mtd impl writeBool(value: bool)        = serializer.writeNative(value)
    mtd impl writeS8(value: s8)            = serializer.writeNative(value)
    mtd impl writeS16(value: s16)          = serializer.writeNative(value)
    mtd impl writeS32(value: s32)          = serializer.writeNative(value)
    mtd impl writeS64(value: s64)          = serializer.writeNative(value)
    mtd impl writeU8(value: u8)            = serializer.writeNative(value)
    mtd impl writeU16(value: u16)          = serializer.writeNative(value)
    mtd impl writeU32(value: u32)          = serializer.writeNative(value)
    mtd impl writeU64(value: u64)          = serializer.writeNative(value)
    mtd impl writeF32(value: f32)          = serializer.writeNative(value)
    mtd impl writeF64(value: f64)          = serializer.writeNative(value)
}