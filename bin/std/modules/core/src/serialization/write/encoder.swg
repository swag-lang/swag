#global export
#global namespace Serialization
using Swag

#[AttrUsage(AttributeUsage.Struct)]
attr Version(v: u32)    // Set the struct version number

#[AttrUsage(AttributeUsage.Struct|AttributeUsage.StructVariable)]
attr NoSerialize()      // Do not serialize a struct or a field

struct EncoderOptions
{
    storeVersion: bool
}

interface IEncoder
{
    beginElement:   mtd(string, typeinfo)
    endElement:     mtd()
    beginValue:     mtd()
    endValue:       mtd()

    write:          mtd(*void, typeinfo)
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
    options:    EncoderOptions
    serializer: T
    started:    bool
}

impl Encoder
{
    mtd write(data: const *void, dataType: typeinfo)
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
                    bufArr := cast(const *u8) ptrArr.buffer
                    loop i: ptrArr.count
                    {
                        write(bufArr, valueType)
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

            // Core.Object
            if structType.fromGeneric == @typeof(Object)
            {
                dataObj := cast(const *Object'String) data // 'String' is fake
                serializer.beginSection()
                defer serializer.endSection()

                serializer.beginElement("type", structType.fields[0].pointedType)
                write(&dataObj.type, structType.fields[0].pointedType)
                serializer.endElement()

                if dataObj.value
                {
                    serializer.beginElement("value", structType.fields[0].pointedType)
                    write(dataObj.value, dataObj.type)
                    serializer.endElement()
                }

                return
            }

            // Write version number
            var version: u32
            if options.storeVersion
            {
                val := Reflection.getAttributeValue(structType, Version, "v")
                if val != null
                    version = cast(u32) val

                serializer.beginElement("$version", u32)
                serializer.beginValue()
                serializer.writeNative(version)
                serializer.endValue()
                serializer.endElement()
            }

            // A structure with an implementation of ISerialize
            itf := @mkinterface(data, type, ISerialize)
            if itf != null
            {
                if itf.write(version, cast(IEncoder) self)
                    return
            }

            // Get all fields to serialize
            var fields: Array'(const *TypeValue)
            visit field: structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize)
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize)
                    continue
                fields.add(field)
            }

            // Start a fields section
            treated = serializer.beginFields(fields, structType, data)
            defer(noerr) serializer.endFields()
            if treated
                return

            // Generic structure save
            visit field: fields
            {
                if !itf or !itf.writeElement(version, field.name, field.pointedType, data + field.offset, cast(IEncoder) self)
                {
                    serializer.beginElement(field.name, field.pointedType)
                    write(data + field.offset, field.pointedType)
                    serializer.endElement()
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

            bufArr := cast(const *u8) data
            loop typeArr.count
            {
                write(bufArr, typeArr.pointedType)
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
        write(@dataof(value), @kindof(value))
        end()
    }
}

impl IEncoder for Encoder
{
    mtd beginElement(name: string, type: typeinfo)
    {
        serializer.beginElement(name, type)
    }

    mtd endElement()
    {
        serializer.endElement()
    }

    mtd beginValue()
    {
        serializer.beginValue()
    }

    mtd endValue()
    {
        serializer.endValue()
    }

    mtd write(data: *void, type: typeinfo)
    {
        Encoder.write(self, data, type)
    }

    mtd writeBufferU8(data: const *u8, count: uint)
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