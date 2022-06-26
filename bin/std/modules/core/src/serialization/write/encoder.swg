#global namespace Serialization
using Swag

public struct Encoder
{
    result:     *ConcatBuffer
    serializer: Write.ISerializer
}

impl Encoder
{
    mtd write(dataType: typeinfo, data: const *u8)
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
                serializer.writeBool(dref cast(const *bool) data)
            case U8:
                serializer.writeU8(dref cast(const *u8) data)
            case U16:
                serializer.writeU16(dref cast(const *u16) data)
            case U32, Rune:
                serializer.writeU32(dref cast(const *u32) data)
            case U64:
                serializer.writeU64(dref cast(const *u64) data)
            case S8:
                serializer.writeS8(dref cast(const *s8) data)
            case S16:
                serializer.writeS16(dref cast(const *s16) data)
            case S32:
                serializer.writeS32(dref cast(const *s32) data)
            case S64:
                serializer.writeS64(dref cast(const *s64) data)
            case F32:
                serializer.writeF32(dref cast(const *f32) data)
            case F64:
                serializer.writeF64(dref cast(const *f64) data)
            case String:
                serializer.writeString(dref cast(const *string) data)
            }

            return
        }

        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            treated := serializer.startSequence(typeArr.pointedType, typeArr.count, data)
            defer serializer.endSequence()

            if !treated
            {
                bufArr := cast(const *u8) data
                loop typeArr.count
                {
                    write(typeArr.pointedType, bufArr)
                    bufArr += typeArr.pointedType.sizeof
                }
            }

            return
        }

        if Reflection.isStruct(type)
        {
            typeStruct := cast(const *TypeInfoStruct) type
            treated := serializer.startStruct(typeStruct, data)
            defer serializer.endStruct()

            if !treated
            {
                visit field: typeStruct.fields
                {
                    serializer.startElement(field)
                    write(field.pointedType, data + field.offset)
                    serializer.endElement(field)
                }
            }
        }
    }

    public mtd init(result: *ConcatBuffer, serializer: Write.ISerializer)
    {
        self.result = #self.result
        self.serializer = #self.serializer
        self.serializer.startWrite(#self.result)
    }

    public mtd write(value: any)
    {
        write(@kindof(value), @dataof(value))
    }
}