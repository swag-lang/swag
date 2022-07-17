#global export
#global namespace Serialization
using Swag

struct DecoderOptions
{
    hasVersion: bool
}

interface IDecoder
{
    read:           func(self, string, *void, typeinfo) throw
    readBufferU8:   func(self, string, uint)->*u8 throw
    readBool:       func(self, string)->bool throw
    readS8:         func(self, string)->s8 throw
    readS16:        func(self, string)->s16 throw
    readS32:        func(self, string)->s32 throw
    readS64:        func(self, string)->s64 throw
    readU8:         func(self, string)->u8 throw
    readU16:        func(self, string)->u16 throw
    readU32:        func(self, string)->u32 throw
    readU64:        func(self, string)->u64 throw
    readF32:        func(self, string)->f32 throw
    readF64:        func(self, string)->f64 throw
    readString:     func(self, string)->String throw
}

struct(T) Decoder
{
    options:    DecoderOptions
    serializer: T
    started:    bool
}

impl Decoder
{
    mtd readVersion()->u32 throw
    {
        if !options.hasVersion
            return 0
        serializer.beginElement("$version", u32)
        serializer.beginValue(u32)
        version := serializer.readNative'u32()
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

            // Core.Object
            if structType.fromGeneric == @typeof(Object)
            {
                dataObj := cast(*Object'String) data // 'String' is fake
                serializer.beginElement("type", structType.fields[0].pointedType)
                read(cast(*u8) &dataObj.type, structType.fields[0].pointedType)
                serializer.endElement()

                if dataObj.type
                {
                    dataObj.value = acast Memory.new(dataObj.type)
                    serializer.beginElement("value", dataObj.type)
                    read(cast(*u8) dataObj.value, dataObj.type)
                    serializer.endElement()
                }

                return
            }

            // A structure with an implementation of ISerialize
            itf := @mkinterface(data, type, ISerialize)
            if itf != null
            {
                itf.read(cast(IDecoder) self)
                return
            }

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
            treated := serializer.beginSequence(typeArr.pointedType, data, &typeArr.count)
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
    mtd(TR) read(input: const [..] u8)->*TR throw
    {
        Debug.assert(!started)
        Debug.assert(Reflection.isStruct(TR))

        start(input)
        val := Memory.new'TR()
        read(cast(*u8) val, TR)
        end()

        return val
    }
}

impl Decoder
{
    mtd(NT) readNative(name: string)->NT throw
    {
        Debug.assert(started)
        serializer.beginElement(name, NT)
        serializer.beginValue(NT)
        res := serializer.readNative'NT()
        serializer.endValue()
        serializer.endElement()
        return res
    }
}

impl IDecoder for Decoder
{
    mtd read(name: string, data: *void, dataType: typeinfo) throw
    {
        Debug.assert(started)
        serializer.beginElement(name, dataType)
        Decoder.read(self, data, dataType)
        serializer.endElement()
    }

    mtd readBufferU8(name: string, count: uint)->*u8 throw
    {
        Debug.assert(started)

        serializer.beginElement(name, *u8)
        defer(noerr) serializer.endElement()

        data := cast(*u8) Memory.alloc(count)

        cpt := count
        treated := serializer.beginSequence(u8, data, &cpt)
        defer(noerr) serializer.endSequence()

        if !treated
        {
            ptr := data
            loop cpt
            {
                serializer.beginValue(u8)
                dref ptr = serializer.readNative'u8()
                serializer.endValue()
                ptr += 1
            }
        }

        return data
    }

    mtd readBool(name: string)->bool throw      => try readNative'bool(name)
    mtd readS8(name: string)->s8 throw          => try readNative's8(name)
    mtd readS16(name: string)->s16 throw        => try readNative's16(name)
    mtd readS32(name: string)->s32 throw        => try readNative's32(name)
    mtd readS64(name: string)->s64 throw        => try readNative's64(name)
    mtd readU8(name: string)->u8 throw          => try readNative'u8(name)
    mtd readU16(name: string)->u16 throw        => try readNative'u16(name)
    mtd readU32(name: string)->u32 throw        => try readNative'u32(name)
    mtd readU64(name: string)->u64 throw        => try readNative'u64(name)
    mtd readF32(name: string)->f32 throw        => try readNative'f32(name)
    mtd readF64(name: string)->f64 throw        => try readNative'f64(name)
    mtd readString(name: string)->String throw  => try readNative'String(name)
}