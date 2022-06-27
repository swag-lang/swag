#global export
#global namespace Serialization
using Swag

struct EncoderOptions
{
    specialRoot: bool = true
}

struct(T) Encoder
{
    options:    EncoderOptions
    serializer: T
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
            }

            return
        }

        if type == String
        {
            strBuf := cast(const *String) data
            serializer.writeString(strBuf.toString())
            return
        }

    // Dynamic Core.Array
    if Reflection.isStruct(type)
    {
        structType := cast(const *TypeInfoStruct) type
        if structType.fromGeneric == @typeof(Core.Array)
        {
            valueType := structType.generics[0].pointedType
            ptrArr    := cast(const *Array) data

            treated := serializer.startSequence(valueType, ptrArr.count, cast(const *u8) ptrArr.buffer)
            defer serializer.endSequence()

            if !treated
            {
                bufArr := cast(const *u8) ptrArr.buffer
                loop ptrArr.count
                {
                    write(valueType, bufArr)
                    bufArr += valueType.sizeof
                }
            }

            return
        }
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

    mtd write(result: *ConcatBuffer, value: any)
    {
        Debug.assert(Reflection.isStruct(@kindof(value)))

        serializer.startWrite(result)

        var rootValue: Swag.TypeValue
        if options.specialRoot
        {
            serializer.startRoot()
            rootValue.name = "root"
            serializer.startElement(&rootValue)
        }

        write(@kindof(value), @dataof(value))

        if options.specialRoot
        {
            serializer.endElement(&rootValue)
            serializer.endRoot()
        }
    }
}