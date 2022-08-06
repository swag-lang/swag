#global export
#global namespace Serialization
using Swag

interface IEncoder
{
    beginField:     mtd(TypeValue)
    endField:       mtd()

    write:          mtd(const *void, typeinfo)
    writeBufferU8:  mtd(const *u8, uint)
    writeBool:      mtd(bool)
    writeS8:        mtd(s8)
    writeS16:       mtd(s16)
    writeS32:       mtd(s32)
    writeS64:       mtd(s64)
    writeU8:        mtd(u8)
    writeU16:       mtd(u16)
    writeU32:       mtd(u32)
    writeU64:       mtd(u64)
    writeF32:       mtd(f32)
    writeF64:       mtd(f64)
    writeString:    mtd(string)
}

struct(T) Encoder
{
    serializer: T
    started:    bool
}

impl Encoder
{
    mtd writeTypeValue(typeVal: TypeValue, data: const ^void)
    {
        serializer.beginElement(typeVal)
        writeValue(data, typeVal.pointedType)
        serializer.endElement()
    }

    mtd writeValue(data: const ^void, dataType: typeinfo)
    {
        type := Reflection.makeConcreteAlias(dataType)
        type = Reflection.makeConcreteEnum(type)

        serializer.beginValue()
        defer(noerr) serializer.endValue()

        // Native
        if type.kind == .Native
        {
            typeNative := cast(const *TypeInfoNative) type
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
            case U64, UInt:
                serializer.writeNative(dref cast(const *u64) data)
            case S8:
                serializer.writeNative(dref cast(const *s8) data)
            case S16:
                serializer.writeNative(dref cast(const *s16) data)
            case S32:
                serializer.writeNative(dref cast(const *s32) data)
            case S64, Int:
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
            typeType := dref cast(const **TypeInfo) data
            serializer.writeNative(typeType ? typeType.crc32 : 0'u32)
            return
        }

        if Reflection.isInterface(type)
        {
            itf := cast(const *Swag.Interface) data

            serializer.beginSection()
            defer serializer.endSection()

            var objType, itfType: typeinfo
            if itf.itable
            {
                objType = itf.itable[bitcast(uint) -1'int]
                itfType = itf.itable[bitcast(uint) -2'int]
            }

            var typeVal: TypeValue
            typeVal.pointedType = typeinfo

            typeVal.name = "objtype"
            typeVal.crc32 = 1
            writeTypeValue(typeVal, &objType)

            if objType
            {
                typeVal.name = "itftype"
                typeVal.crc32 = 2
                writeTypeValue(typeVal, &itfType)

                typeVal.name = "value"
                typeVal.crc32 = 3
                typeVal.pointedType = objType

                writeTypeValue(typeVal, itf.obj)
            }

            return
        }

        if Reflection.isStruct(type)
        {
            // Core.String
            if type == String
            {
                strBuf := cast(const *String) data
                serializer.writeNative(strBuf.toString())
                return
            }

            structType := cast(const *TypeInfoStruct) type

            // Core.Array
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType
                ptrArr    := cast(const *Array) data

                treated := serializer.beginSequence(type, data, ptrArr.count)
                defer serializer.endSequence()

                if !treated
                {
                    bufArr := cast(const ^u8) ptrArr.buffer
                    loop i: ptrArr.count
                    {
                        writeValue(bufArr, valueType)
                        bufArr += valueType.sizeof
                    }
                }

                return
            }

            // Generic structure save
            treated := serializer.beginStruct(structType, data)
            defer(noerr) serializer.endStruct()
            if treated
                return

            // A structure with an implementation of ISerialize
            itf := @mkinterface(data, type, ISerialize)
            if itf != null
            {
                if itf.write(cast(IEncoder) self)
                    return
            }

            // Start a fields section
            treated = serializer.beginFields(structType, data)
            defer(noerr) serializer.endFields()
            if treated
                return

            // Generic structure save
            visit field: structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize)
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize)
                    continue

                if !itf or !itf.writeElement(dref field, data + field.offset, cast(IEncoder) self)
                {
                    writeTypeValue(field, data + field.offset)
                }
            }

            return
        }

        // A static array
        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            treated := serializer.beginSequence(typeArr.pointedType, data, typeArr.count)
            defer serializer.endSequence()
            if treated
                return

            bufArr := cast(const ^u8) data
            loop typeArr.count
            {
                writeValue(bufArr, typeArr.pointedType)
                bufArr += typeArr.pointedType.sizeof
            }

            return
        }
    }

    // Start serialization
    mtd start(result: *ConcatBuffer)
    {
        Debug.assert(!started)
        started = true
        serializer.startWrite(result)
        serializer.beginRoot()
    }

    // End serialization
    mtd end()
    {
        Debug.assert(started)
        started = false
        serializer.endRoot()
    }

    // Write a full struct
    mtd writeAll(result: *ConcatBuffer, value: any)
    {
        Debug.assert(!started)
        Debug.assert(Reflection.isStruct(@kindof(value)))
        start(result)
        writeValue(@dataof(value), @kindof(value))
        end()
    }
}

impl IEncoder for Encoder
{
    mtd beginField(val: Swag.TypeValue)
    {
        serializer.beginElement(val)
        serializer.beginValue()
    }

    mtd endField()
    {
        serializer.endValue()
        serializer.endElement()
    }

    mtd write(data: const ^void, type: typeinfo)
    {
        writeValue(data, type)
    }

    mtd writeBufferU8(data: const ^u8, count: uint)
    {
        treated := serializer.beginSequence(u8, data, count)
        defer(noerr) serializer.endSequence()

        if !treated
        {
            ptr := data
            loop count
            {
                serializer.beginValue()
                serializer.writeNative(dref ptr)
                serializer.endValue()
                ptr += 1
            }
        }
    }

    mtd writeBool(value: bool)        = serializer.writeNative(value)
    mtd writeS8(value: s8)            = serializer.writeNative(value)
    mtd writeS16(value: s16)          = serializer.writeNative(value)
    mtd writeS32(value: s32)          = serializer.writeNative(value)
    mtd writeS64(value: s64)          = serializer.writeNative(value)
    mtd writeU8(value: u8)            = serializer.writeNative(value)
    mtd writeU16(value: u16)          = serializer.writeNative(value)
    mtd writeU32(value: u32)          = serializer.writeNative(value)
    mtd writeU64(value: u64)          = serializer.writeNative(value)
    mtd writeF32(value: f32)          = serializer.writeNative(value)
    mtd writeF64(value: f64)          = serializer.writeNative(value)
    mtd writeString(value: string)    = serializer.writeNative(value)
}