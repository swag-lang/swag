#global export
#global namespace Serialization
using Swag

struct(T) Decoder
{
    serializer: T
}

impl Decoder
{
    mtd read(dataType: typeinfo, data: const *u8) throw
    {
        type := Reflection.makeConcreteAlias(dataType)

        serializer.startValue(type)
        defer serializer.endValue(type)

        if type.kind == .Native
        {
            typeNative := cast(const *TypeInfoNative) type
            switch typeNative.nativeKind
            {
            case Bool:
                dref cast(const *bool) data = serializer.readBool()
            case U8:
                dref cast(const *u8) data = serializer.readU8()
            case U16:
                dref cast(const *u16) data = serializer.readU16()
            case U32, Rune:
                dref cast(const *u32) data = serializer.readU32()
            case U64:
                dref cast(const *u64) data = serializer.readU64()
            case S8:
                dref cast(const *s8) data = serializer.readS8()
            case S16:
                dref cast(const *s16) data = serializer.readS16()
            case S32:
                dref cast(const *s32) data = serializer.readS32()
            case S64:
                dref cast(const *s64) data = serializer.readS64()
            case F32:
                dref cast(const *f32) data = serializer.readF32()
            case F64:
                dref cast(const *f64) data = serializer.readF64()
            case String:
                dref cast(const *string) data = serializer.readString()
            }

            return
        }

        if Reflection.isStruct(type)
        {
            typeStruct := cast(const *TypeInfoStruct) type
            treated := serializer.startStruct(type, data)
            defer serializer.endStruct()

            if !treated
            {
                visit field: typeStruct.fields
                {
                    serializer.startElement(field)
                    read(field.pointedType, data + field.offset)
                    serializer.endElement(field)
                }
            }
        }
    }

    mtd read(input: const [..] u8, type: typeinfo)->*u8 throw
    {
        Debug.assert(Reflection.isStruct(type))

        serializer.startRead(input)

        val := Memory.new(type)
        read(type, val)
        return val
    }
}