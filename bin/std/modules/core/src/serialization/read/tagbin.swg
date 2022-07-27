#global namespace Serialization.Read
#global public
using Swag

struct TagBinSection
{
    mapSeek: HashTable'(u32, uint)
    raw:     bool
    final:   bool
}

struct TagBin
{
    stream:   ByteStream
    sections: Array'TagBinSection
}

impl TagBin
{
    mtd startRead(input: const [..] u8) throw
    {
        stream = ByteStream.openRead(input)
    }

    mtd beginRoot() throw
    {
        sig := stream.readNative'u32()
        if sig != Write.TagBinSignatures.Root
            throw "invalid file"
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

    mtd beginSection()
    {
        sec := sections.emplaceAddress(1)
        @init(sec)
        sec.raw = true
    }

    mtd endSection()
    {
        sections.removeBack()
    }

    mtd beginFields(fields: Array'(const *TypeValue), structType: typeinfo, structData: const *u8)->bool throw
    {
        if Reflection.hasAttribute(structType, Serialization.Final)
        {
            sec := sections.emplaceAddress(1)
            @init(sec)
            sec.final = true
            return false
        }

        sig := stream.readNative'u32()
        if sig != Write.TagBinSignatures.Fat
            throw "invalid fat signature"

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

    mtd endFields(fields: Array'(const *TypeValue)) throw
    {
        sections.removeBack()
    }

    mtd beginElement(val: TypeValue)->bool throw
    {
        // Outside a section
        if !sections.isEmpty()
        {
            sec := sections.backPtr()

            // If section is final, then we just have values without versionning
            if sec.final
                return true

            // If section is raw, then we do not have a fat, this are not fields
            if !sec.raw
            {
                ent := sec.mapSeek.find(val.crc32)

                // If ent is null, then we are requesting a field that do not exist
                // in the stream
                if !ent
                    return false

                // Seek to the corresponding start of element
                stream.setSeek(ent.value)
            }
        }

        // Read element signature
        crcName := stream.readNative'u32()
        if crcName != val.crc32
            throw "stream crc name invalid"
        crcType := stream.readNative'u32()

        return true
    }

    mtd endElement() throw
    {
    }

    mtd beginValue() throw
    {
    }

    mtd endValue() throw
    {
    }

    mtd beginSequence(typeElem: typeinfo, data: *u8, countElem: *uint)->bool throw
    {
        sig := stream.readNative'u32()
        if sig != Write.TagBinSignatures.Sequence
            throw "invalid sequence signature"

        // 0 for a dynamic array, > 0 for a static array
        orgCount := dref countElem
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
            dref countElem = Math.min(orgCount, dref countElem)

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