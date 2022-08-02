#global export
#global namespace Serialization
using Swag

interface IDecoder
{
    beginField:     mtd(TypeValue)->bool throw
    endField:       mtd() throw
    getVersion:     mtd()->u32

    read:           mtd(*void, typeinfo) throw
    readBufferU8:   mtd(uint)->*u8 throw
    readBool:       mtd()->bool throw
    readS8:         mtd()->s8 throw
    readS16:        mtd()->s16 throw
    readS32:        mtd()->s32 throw
    readS64:        mtd()->s64 throw
    readU8:         mtd()->u8 throw
    readU16:        mtd()->u16 throw
    readU32:        mtd()->u32 throw
    readU64:        mtd()->u64 throw
    readF32:        mtd()->f32 throw
    readF64:        mtd()->f64 throw
    readString:     mtd()->String throw
}

struct(T) Decoder
{
    serializer: T
    started:    bool
}

impl Decoder
{
    mtd readValue(data: ^void, dataType: typeinfo) throw
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
                dref cast(*bool) data = serializer.readNative'bool()
            case U8:
                dref cast(*u8) data   = serializer.readNative'u8()
            case U16:
                dref cast(*u16) data  = serializer.readNative'u16()
            case U32, Rune:
                dref cast(*u32) data  = serializer.readNative'u32()
            case U64, UInt:
                dref cast(*u64) data  = serializer.readNative'u64()
            case S8:
                dref cast(*s8) data   = serializer.readNative's8()
            case S16:
                dref cast(*s16) data  = serializer.readNative's16()
            case S32:
                dref cast(*s32) data  = serializer.readNative's32()
            case S64, Int:
                dref cast(*s64) data  = serializer.readNative's64()
            case F32:
                dref cast(*f32) data  = serializer.readNative'f32()
            case F64:
                dref cast(*f64) data  = serializer.readNative'f64()
            }

            return
        }

        if Reflection.isType(type)
        {
            crc := serializer.readNative'u32()
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
                dref strBuf =,nodrop serializer.readNative'String()
                return
            }

            // Core.Array
            structType := cast(const *TypeInfoStruct) type
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType

                count := 0'uint
                treated := serializer.beginSequence(type, data, &count)
                defer(noerr) serializer.endSequence()

                if !treated
                {
                    while serializer.toNextSequenceElement(&count)
                    {
                        bufArr := Reflection.roomInArray(data, structType, valueType)
                        readValue(bufArr, valueType)
                    }
                }

                return
            }

            // Generic struct load
            treated := serializer.beginStruct(data, structType)
            defer(noerr) serializer.endStruct()
            if treated
                return

            // Core.Object
            if structType.fromGeneric == @typeof(Object)
            {
                dataObj := cast(*Object'String) data // 'String' is fake

                serializer.beginSection()
                defer serializer.endSection()

                var typeVal: TypeValue

                typeVal.name = "type"
                typeVal.crc32 = 1
                typeVal.pointedType = structType.fields[0].pointedType

                discard serializer.beginElement(typeVal)
                readValue(&dataObj.type, typeVal.pointedType)
                serializer.endElement()

                typeVal.name = "value"
                typeVal.crc32 = 2
                typeVal.pointedType = dataObj.type

                if dataObj.type
                {
                    dataObj.value = acast Memory.new(dataObj.type)

                    discard serializer.beginElement(typeVal)
                    readValue(dataObj.value, typeVal.pointedType)
                    serializer.endElement()
                }

                return
            }

            // A structure with an implementation of ISerialize
            idecoder := cast(IDecoder) self
            itf := @mkinterface(data, type, ISerialize)
            if itf != null
            {
                if itf.read(idecoder)
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

            // Generic struct load
            treated = serializer.beginFields(fields, structType, data)
            defer(noerr) serializer.endFields(fields)
            if treated
                return

            visit field: fields
            {
                if !itf or !itf.readElement(dref field, data + field.offset, idecoder)
                {
                    if serializer.beginElement(field)
                    {
                        readValue(data + field.offset, field.pointedType)
                        serializer.endElement()
                    }
                }
            }

            // Inform user code we are done
            if itf
                itf.postSerialize(data, idecoder)

            return
        }

        // Static array
        if Reflection.isArray(type)
        {
            typeArr := cast(const *TypeInfoArray) type
            count   := typeArr.count
            treated := serializer.beginSequence(typeArr.pointedType, data, &count)
            defer(noerr) serializer.endSequence()
            if treated
                return

            bufArr := cast(^u8) data
            loop count
            {
                readValue(bufArr, typeArr.pointedType)
                bufArr += typeArr.pointedType.sizeof
            }

            return
        }
    }

    // Start reading
    mtd start(input: const [..] u8) throw
    {
        Debug.assert(!started)
        started = true
        serializer.startRead(input)
        serializer.beginRoot()
    }

    // Finish reading
    mtd end() throw
    {
        Debug.assert(started)
        started = false
        serializer.endRoot()
    }

    // Read a full struct
    mtd(TR) readAll(input: const [..] u8)->*TR throw
    {
        Debug.assert(!started)
        Debug.assert(Reflection.isStruct(TR))

        start(input)

        val := Memory.new'TR()
        defer(err) Memory.delete(val)

        readValue(val, TR)

        end()
        return val
    }
}

impl IDecoder for Decoder
{
    mtd beginField(val: Swag.TypeValue)->bool throw
    {
        if !serializer.beginElement(val)
            return false
        serializer.beginValue()
        return true
    }

    mtd getVersion()->u32
    {
        return serializer.getVersion()
    }

    mtd endField() throw
    {
        serializer.endValue()
        serializer.endElement()
    }

    mtd read(data: *void, dataType: typeinfo) throw
    {
        readValue(data, dataType)
    }

    mtd readBufferU8(count: uint)->*u8 throw
    {
        data := cast(^u8) Memory.alloc(count)

        cpt := count
        treated := serializer.beginSequence(u8, data, &cpt)
        defer(noerr) serializer.endSequence()

        if !treated
        {
            ptr := data
            loop Math.min(cpt, count)
            {
                serializer.beginValue()
                dref ptr = serializer.readNative'u8()
                serializer.endValue()
                ptr += 1
            }
        }

        return data
    }

    mtd readBool()->bool throw      => serializer.readNative'bool()
    mtd readS8()->s8 throw          => serializer.readNative's8()
    mtd readS16()->s16 throw        => serializer.readNative's16()
    mtd readS32()->s32 throw        => serializer.readNative's32()
    mtd readS64()->s64 throw        => serializer.readNative's64()
    mtd readU8()->u8 throw          => serializer.readNative'u8()
    mtd readU16()->u16 throw        => serializer.readNative'u16()
    mtd readU32()->u32 throw        => serializer.readNative'u32()
    mtd readU64()->u64 throw        => serializer.readNative'u64()
    mtd readF32()->f32 throw        => serializer.readNative'f32()
    mtd readF64()->f64 throw        => serializer.readNative'f64()
    mtd readString()->String throw  => serializer.readNative'String()
}