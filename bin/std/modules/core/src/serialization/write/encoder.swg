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
        type = Reflection.makeConcreteEnum(type)

        serializer.startValue(type)
        defer(noerr) serializer.endValue(type)

        // Native
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

        if Reflection.isStruct(type)
        {
            // Core.String
            if type == String
            {
                strBuf := cast(const *String) data
                serializer.writeString(strBuf.toString())
                return
            }

            // Core.Array
            structType := cast(const *TypeInfoStruct) type
            if structType.fromGeneric == @typeof(Array)
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

            // Generic structure save
            treated := serializer.startStruct(structType, data)
            defer(noerr) serializer.endStruct()
            if treated
                return

            // Write version number
            if options.storeVersion
            {
                var version: u32
                val := Reflection.getAttributeValue(structType, Version, "v")
                if val != null
                    version = cast(u32) val

                var fv: TypeValue
                fv.name = "__version"
                serializer.startElement(fv)
                serializer.startValue(u32)
                serializer.writeU32(version)
                serializer.endValue(u32)
                serializer.endElement(fv)
            }

            visit field: structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize)
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize)
                    return

                serializer.startElement(field)
                write(field.pointedType, data + field.offset)
                serializer.endElement(field)
            }

            return
        }

        // A static array
        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            treated := serializer.startSequence(typeArr.pointedType, typeArr.count, data)
            defer serializer.endSequence()
            if treated
                return

            bufArr := cast(const *u8) data
            loop typeArr.count
            {
                write(typeArr.pointedType, bufArr)
                bufArr += typeArr.pointedType.sizeof
            }

            return
        }
    }

    mtd write(result: *ConcatBuffer, value: any)
    {
        Debug.assert(Reflection.isStruct(@kindof(value)))
        serializer.startWrite(result)
        write(@kindof(value), @dataof(value))
    }
}