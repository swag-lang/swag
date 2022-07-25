#global namespace Serialization.Read
#global public
using Swag

struct TagBinSection
{
    mapSeek: HashTable'(u32, uint)
}

struct TagBin
{
    stream:   ByteStream
    sections: Array'TagBinSection
}

impl TagBin
{
    mtd startRead(input: const [..] u8)
    {
        stream.init(input)
    }

    mtd beginRoot() throw
    {
    }

    mtd endRoot() throw
    {
    }

    mtd beginStruct(data: *u8, type: typeinfo)->bool throw
    {
        return false
    }

    mtd endStruct() throw
    {
    }

    mtd beginFields(fields: Array'(const *TypeValue), structData: *u8, structType: typeinfo)->bool throw
    {
        sig := stream.readNative'u32()
        if sig != Write.TagBinSignatures.SignatureFat
            throw "bad SignatureFat"

        // Read fat
        sec := sections.emplaceAddress(1)
        @init(sec)

        fatCount := stream.readNative'uint()
        while fatCount
        {
            crc  := stream.readNative'u32()
            seek := stream.readNative'uint()
            sec.mapSeek.add(crc, seek)
            fatCount -= 1
        }

        return false
    }

    mtd endFields() throw
    {
        sections.removeBack()
    }

    mtd beginElement(name: string, type: typeinfo) throw
    {
        // Outside a section
        if sections.isEmpty()
            return

        sec := sections.backPtr()
        crc := getCrc(name)
        Debug.assert(sec.mapSeek.contains(crc))
    }

    mtd endElement()
    {
    }

    mtd beginValue(type: typeinfo)
    {
    }

    mtd endValue()
    {
    }

    mtd beginSequence(typeElem: typeinfo, data: *u8, countElem: *uint)->bool throw
    {
        dref countElem = stream.readNative'uint()

        // Dynamic array of raw serializable types
        // Just read them in one batch
        if typeElem.kind == .Struct
        {
            structType := cast(const *TypeInfoStruct) typeElem
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType
                (raw, sizeof) := isRawSerializable(valueType)

                if raw
                {
                    bufArr := cast(*Array'u8) data
                    bufArr.count = dref countElem
                    bufArr.reserve(bufArr.count * sizeof)
                    bufArr.capacity = bufArr.count
                    stream.readBytes(bufArr.buffer, bufArr.count * sizeof)
                    return true
                }
            }
        }

        // Static array of raw serializable types
        else
        {
            (raw, sizeof) := isRawSerializable(typeElem)
            if raw
            {
                stream.readBytes(data, dref countElem * sizeof)
                return true
            }
        }

        return false
    }

    mtd endSequence() throw
    {
    }

    mtd toNextSequenceElement(countElem: *uint)->bool
    {
        if dref countElem == 0
            return false
        dref countElem -= 1
        return true
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd(NT) readNative()->NT throw
    {
        #if NT == Core.String
        {
            var result: retval
            count := cast(uint) stream.readNative'u32()
            result.reserve(count)
            stream.readBytes(result.buffer, count)
            result.length = count
            return result
        }
        #else
        {
            return stream.readNative'NT()
        }
    }
}