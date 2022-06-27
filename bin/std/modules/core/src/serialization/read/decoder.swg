#global export
#global namespace Serialization
using Swag

struct(T) Decoder
{
    serializer: T
}

impl Decoder
{
    mtd read(data: *u8, dataType: typeinfo) throw
    {
        type := Reflection.makeConcreteAlias(dataType)

        serializer.startValue(type)
        defer(noerr) serializer.endValue(type)

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

        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            treated := serializer.startSequence(data, typeArr.count, typeArr.pointedType)
            defer(noerr) serializer.endSequence()

            if !treated
            {
                bufArr := cast(*u8) data
                loop typeArr.count
                {
                    read(bufArr, typeArr.pointedType)
                    bufArr += typeArr.pointedType.sizeof
                }
            }

            return
        }

        if Reflection.isStruct(type)
        {
            typeStruct := cast(const *TypeInfoStruct) type
            treated := serializer.startStruct(data, type)
            defer(noerr) serializer.endStruct()

            if !treated
            {
                visit field: typeStruct.fields
                {
                    serializer.startElement(field)
                    read(data + field.offset, field.pointedType)
                    serializer.endElement(field)
                }
            }
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