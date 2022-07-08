#global export
#global namespace Serialization
using Swag

struct DecoderOptions
{
    hasVersion: bool
}

struct(T) Decoder
{
    options:    DecoderOptions
    serializer: T
}

impl Decoder
{
    mtd readVersion()->u32 throw
    {
        if !options.hasVersion
            return 0
        serializer.beginElement("$version", u32)
        serializer.beginValue(u32)
        version := serializer.readU32()
        serializer.endValue()
        serializer.endElement()
        return version
    }

    mtd read(data: *u8, dataType: typeinfo) throw
    {
        type := Reflection.makeConcreteAlias(dataType)
        type = Reflection.makeConcreteEnum(type)

        serializer.beginValue(type)
        defer(noerr) serializer.endValue()

        // Native
        if type.kind == .Native
        {
            typeNative := cast(const *TypeInfoNative) type
            switch typeNative.nativeKind
            {
            case Bool:
                dref cast(*bool) data = serializer.readBool()
            case U8:
                dref cast(*u8) data = serializer.readU8()
            case U16:
                dref cast(*u16) data = serializer.readU16()
            case U32, Rune:
                dref cast(*u32) data = serializer.readU32()
            case U64:
                dref cast(*u64) data = serializer.readU64()
            case S8:
                dref cast(*s8) data = serializer.readS8()
            case S16:
                dref cast(*s16) data = serializer.readS16()
            case S32:
                dref cast(*s32) data = serializer.readS32()
            case S64:
                dref cast(*s64) data = serializer.readS64()
            case F32:
                dref cast(*f32) data = serializer.readF32()
            case F64:
                dref cast(*f64) data = serializer.readF64()
            }

            return
        }

        if Reflection.isType(type)
        {
            crc := serializer.readU32()
            typeType := Reflection.crcToType(crc)
            dref cast(*typeinfo) data = typeType
            return
        }

        if Reflection.isStruct(type)
        {
            // Core.String
            if type == String
            {
                strBuf := cast(*String) data
                dref strBuf =,nodrop serializer.readString()
                return
            }

            // Core.Array
            structType := cast(const *TypeInfoStruct) type
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType

                treated := serializer.beginSequence(data, 0, type)
                defer(noerr) serializer.endSequence()

                if !treated
                {
                    while serializer.toNextSequenceElement()
                    {
                        bufArr := Reflection.roomInArray(data, structType, valueType)
                        read(bufArr, valueType)
                    }
                }

                return
            }

            // Generic struct load
            treated := serializer.beginStruct(data, structType)
            defer(noerr) serializer.endStruct()
            if treated
                return

            version := readVersion()
            visit field: structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize)
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize)
                    return

                serializer.beginElement(field.name, field.pointedType)
                read(data + field.offset, field.pointedType)
                serializer.endElement()
            }

            return
        }

        // Static array
        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            treated := serializer.beginSequence(data, typeArr.count, typeArr.pointedType)
            defer(noerr) serializer.endSequence()
            if treated
                return

            bufArr := cast(*u8) data
            loop typeArr.count
            {
                read(bufArr, typeArr.pointedType)
                bufArr += typeArr.pointedType.sizeof
            }

            return
        }
    }

    mtd(TR) read(input: const [..] u8)->*TR throw
    {
        Debug.assert(Reflection.isStruct(TR))

        serializer.startRead(input)
        val := Memory.new'TR()
        read(cast(*u8) val, TR)

        return val
    }
}