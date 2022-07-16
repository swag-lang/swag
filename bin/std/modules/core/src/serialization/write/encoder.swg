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
    write: func(self, string, any)
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

        serializer.beginValue(type)
        defer(noerr) serializer.endValue()

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

        if Reflection.isType(type)
        {
            typeType := dref cast(const **TypeInfo) data
            serializer.writeU32(typeType ? typeType.crc32 : 0)
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

            structType := cast(const *TypeInfoStruct) type

            // Core.Array
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType
                ptrArr    := cast(const *Array) data

                treated := serializer.beginSequence(valueType, ptrArr.count, cast(const *u8) ptrArr.buffer)
                defer serializer.endSequence()

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
            treated := serializer.beginStruct(structType, data)
            defer(noerr) serializer.endStruct()
            if treated
                return

            // Core.Object
            if structType.fromGeneric == @typeof(Object)
            {
                dataObj := cast(const *Object'String) data // 'String' is fake
                serializer.beginElement("type", structType.fields[0].pointedType)
                write(structType.fields[0].pointedType, cast(const *u8) &dataObj.type)
                serializer.endElement()

                if dataObj.value
                {
                    serializer.beginElement("value", structType.fields[0].pointedType)
                    write(dataObj.type, cast(const *u8) dataObj.value)
                    serializer.endElement()
                }

                return
            }

            // A structure with an implementation of ISerialize
            itf := @mkinterface(data, type, ISerialize)
            if itf != null
            {
                itf.write(cast(IEncoder) self)
                return
            }

            // Write version number
            if options.storeVersion
            {
                var version: u32
                val := Reflection.getAttributeValue(structType, Version, "v")
                if val != null
                    version = cast(u32) val

                serializer.beginElement("$version", u32)
                serializer.beginValue(u32)
                serializer.writeU32(version)
                serializer.endValue()
                serializer.endElement()
            }

            // Get all fields to serialize
            visit field: structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize)
                    continue
                if Reflection.hasAttribute(field.pointedType, NoSerialize)
                    continue

                serializer.beginElement(field.name, field.pointedType)
                write(field.pointedType, data + field.offset)
                serializer.endElement()
            }

            return
        }

        // A static array
        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            treated := serializer.beginSequence(typeArr.pointedType, typeArr.count, data)
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
}

impl IEncoder for Encoder
{
    // Write an element
    mtd write(name: string, value: any)
    {
        Debug.assert(started)
        serializer.beginElement(name, @kindof(value))
        write(@kindof(value), @dataof(value))
        serializer.endElement()
    }
}