#global namespace Serialization.Read
#global public
using Swag

/*
    Binary serializer with forward/backward compatibility
    Changes that are supported from V to V+1 are :

    - Adding a new field. The new field will stay at the default value
    - Removing a field. It will just be ignored
    - Reordering of fields, as long as their name/type are the same
    - Renaming of a field with 'Serialization.Alias'

    Supported attributes are :

    - Serialization.Alias   To rename a field. Multiple aliases are supported.
    - Serialization.Final   The struct will be loaded/saved in place without versioning
                            (faster, lower serialization size)
*/


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

private impl TagBin
{
    func canConvertType(to, from: typeinfo)->bool
    {
        // Conversion of a static array to another static array
        // Are supported :
        // - Size change
        if Reflection.isArray(to) and Reflection.isArray(from)
        {
            toArr   := cast(const *Swag.TypeInfoArray) to
            fromArr := cast(const *Swag.TypeInfoArray) from
            if toArr.pointedType == fromArr.pointedType
                return true
        }

        return false
    }
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
        reqCrcName := val.crc32

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
                {
                    // So check for aliases
                    aliases := Reflection.getAttributes(val, Serialization.Alias)
                    visit al: aliases
                    {
                        val := Reflection.getAttributeValue(al, "name")
                        Debug.assert(val != null)
                        reqCrcName = Hash.Crc32.compute(cast(string) val)
                        ent = sec.mapSeek.find(reqCrcName)
                        if ent
                            break
                    }
                }

                if !ent
                    return false
                stream.setSeek(ent.value)
            }
        }

        // Read name signature
        crcName := stream.readNative'u32()
        if crcName != reqCrcName
        {
            throw Format.toString("field `%`: names do not match", val.name)
        }

        // Read type signature
        crcType := stream.readNative'u32()
        if crcType != val.pointedType.crc32
        {
            streamType := Reflection.crcToType(crcType)
            if !streamType
                throw Format.toString("field `%`: unknown type", val.name)

            if !canConvertType(val.pointedType, streamType)
                throw Format.toString("field `%`: types do not match", val.name)
        }

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