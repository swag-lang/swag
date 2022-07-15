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
    started:    bool
}

impl Encoder
{
    mtd write(dataType: typeinfo, data: const *u8)
    {
        type := Reflection.makeConcreteAlias(dataType)
        type = Reflection.makeConcreteEnum(type)

        serializer.IWrite.beginValue(type)
        defer(noerr) serializer.IWrite.endValue()

        // Native
        if type.kind == .Native
        {
            typeNative := cast(const *TypeInfoNative) type
            switch typeNative.nativeKind
            {
            case Bool:
                serializer.IWrite.writeBool(dref cast(const *bool) data)
            case U8:
                serializer.IWrite.writeU8(dref cast(const *u8) data)
            case U16:
                serializer.IWrite.writeU16(dref cast(const *u16) data)
            case U32, Rune:
                serializer.IWrite.writeU32(dref cast(const *u32) data)
            case U64:
                serializer.IWrite.writeU64(dref cast(const *u64) data)
            case S8:
                serializer.IWrite.writeS8(dref cast(const *s8) data)
            case S16:
                serializer.IWrite.writeS16(dref cast(const *s16) data)
            case S32:
                serializer.IWrite.writeS32(dref cast(const *s32) data)
            case S64:
                serializer.IWrite.writeS64(dref cast(const *s64) data)
            case F32:
                serializer.IWrite.writeF32(dref cast(const *f32) data)
            case F64:
                serializer.IWrite.writeF64(dref cast(const *f64) data)
            }

            return
        }

        if Reflection.isType(type)
        {
            typeType := dref cast(const **TypeInfo) data
            serializer.IWrite.writeU32(typeType ? typeType.crc32 : 0)
            return
        }

        if Reflection.isStruct(type)
        {
            // Core.String
            if type == String
            {
                strBuf := cast(const *String) data
                serializer.IWrite.writeString(strBuf.toString())
                return
            }

            structType := cast(const *TypeInfoStruct) type

            // Core.Array
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType
                ptrArr    := cast(const *Array) data

                treated := serializer.IWrite.beginSequence(valueType, ptrArr.count, cast(const *u8) ptrArr.buffer)
                defer serializer.IWrite.endSequence()

                if !treated
                {
                    bufArr := cast(const *u8) ptrArr.buffer
                    loop i: ptrArr.count
                    {
                        write(valueType, bufArr)
                        bufArr += valueType.sizeof
                    }
                }

                return
            }

            // Generic structure save
            treated := serializer.IWrite.beginStruct(structType, data)
            defer(noerr) serializer.IWrite.endStruct()
            if treated
                return

            // Core.Object
            if structType.fromGeneric == @typeof(Object)
            {
                dataObj := cast(const *Object'String) data // 'String' is fake
                serializer.IWrite.beginElement("type", structType.fields[0].pointedType)
                write(structType.fields[0].pointedType, cast(const *u8) &dataObj.type)
                serializer.IWrite.endElement()

                if dataObj.value
                {
                    serializer.IWrite.beginElement("value", structType.fields[0].pointedType)
                    write(dataObj.type, cast(const *u8) dataObj.value)
                    serializer.IWrite.endElement()
                }

                return
            }

            // Write version number
            if options.storeVersion
            {
                var version: u32
                val := Reflection.getAttributeValue(structType, Version, "v")
                if val != null
                    version = cast(u32) val

                serializer.IWrite.beginElement("$version", u32)
                serializer.IWrite.beginValue(u32)
                serializer.IWrite.writeU32(version)
                serializer.IWrite.endValue()
                serializer.IWrite.endElement()
            }

            // Get all fields to serialize
            visit field: structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize)
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize)
                    continue

                serializer.IWrite.beginElement(field.name, field.pointedType)
                write(field.pointedType, data + field.offset)
                serializer.IWrite.endElement()
            }

            return
        }

        // A static array
        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            treated := serializer.IWrite.beginSequence(typeArr.pointedType, typeArr.count, data)
            defer serializer.IWrite.endSequence()
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
    mtd write(result: *ConcatBuffer, value: any)
    {
        Debug.assert(!started)
        Debug.assert(Reflection.isStruct(@kindof(value)))
        start(result)
        write(@kindof(value), @dataof(value))
        end()
    }

    // Write an element
    mtd write(name: string, value: any)
    {
        Debug.assert(started)
        serializer.IWrite.beginElement(name, @kindof(value))
        write(@kindof(value), @dataof(value))
        serializer.IWrite.endElement()
    }
}