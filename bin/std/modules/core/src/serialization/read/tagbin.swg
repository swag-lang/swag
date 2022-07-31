#global namespace Serialization.Read
#global public
using Swag

/*
    Binary serializer with forward/backward compatibility
    Changes that are supported from V to V+1 are :

    - Add a new field. The new field will stay at the default value
    - Remove a field. It will just be ignored
    - Reorder of fields, as long as their name/type are the same
    - Rename a field with 'Serialization.Alias'

    Supported type changes are :
    - Change the size of a static array
    - Convert a static array to a dynamic array, and vice versa

    Supported attributes are :

    - Serialisation.Version     If version is specified, then loading will not have versioning if versions are the same
    - Serialization.Alias       To rename a field. Multiple aliases are supported.
    - Serialization.Final       The struct will be loaded/saved without versioning (faster and lower serialization size)
*/

struct TagBinSection
{
    mapSeek: HashTable'(u32, uint)
    version: u32
    raw:     bool
    final:   bool
}

struct TagBinOptions
{
    tryForward: bool    // Accept to try to read a newest version, otherwise error
}

struct TagBin
{
    stream:         ByteStream
    options:        TagBinOptions
    freeSections:   ArrayPtr'TagBinSection
    sections:       ArrayPtr'TagBinSection
}

impl TagBin
{
    const ErrorInvalidFile      = "invalid file"
    const ErrorCorrupted        = "stream is corrupted"
    const ErrorFat              = "invalid fat signature"
    const ErrorNewestVersion    = "version is too recent"
    const ErrorBadType          = "bad field type"
}

private impl TagBin
{
    mtd newSection()->*TagBinSection
    {
        if freeSections.count
        {
            ptr := freeSections.popBack()
            ptr.mapSeek.clear()
            ptr.version = 0
            ptr.raw     = false
            ptr.final   = false
            sections.add(ptr)
            return ptr
        }

        return sections.addNewPtr()
    }

    mtd releaseSection()
    {
        ptr := sections.popBack()
        freeSections.add(ptr)
    }

    func canConvertType(to, from: typeinfo)->bool
    {
        if to == from
            return true

        if Reflection.isArray(from)
        {
            fromArr := cast(const *Swag.TypeInfoArray) from

            // Array to array, this is fine, we will deal with size change later
            if Reflection.isArray(to)
            {
                toArr := cast(const *Swag.TypeInfoArray) to
                if canConvertType(toArr.pointedType, fromArr.pointedType)
                    return true
            }

            // Static array to dynamic array
            if Reflection.isStruct(to)
            {
                toStruct := cast(const *Swag.TypeInfoStruct) to
                if toStruct.fromGeneric == @typeof(Array) and canConvertType(toStruct.generics[0].pointedType, fromArr.pointedType)
                    return true
            }
        }

        if Reflection.isStruct(from)
        {
            fromStruct := cast(const *Swag.TypeInfoStruct) from
            if fromStruct.fromGeneric == @typeof(Array)
            {
                // Dynamic array to Static array
                if Reflection.isArray(to)
                {
                    toArr := cast(const *Swag.TypeInfoArray) to
                    if canConvertType(toArr.pointedType, fromStruct.generics[0].pointedType)
                        return true
                }
            }
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
            throw ErrorInvalidFile
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
        sec := newSection()
        sec.raw = true
    }

    mtd endSection()
    {
        releaseSection()
    }

    mtd beginFields(fields: Array'(const *TypeValue), structType: typeinfo, structData: const *u8)->bool throw
    {
        sec := newSection()

        if Reflection.hasAttribute(structType, Serialization.Final)
        {
            sec.final = true
            return false
        }

        // Read version
        sig := stream.readNative'u32()
        if sig != Write.TagBinSignatures.Version
            throw ErrorCorrupted
        sec.version = stream.readNative'u32()

        // Deal with version
        version := Reflection.getAttributeValue(structType, Serialization.Version, "v")
        if version != null
        {
            curVersion    := cast(u32) version
            streamVersion := sec.version
            if !options.tryForward and curVersion < streamVersion
                throw ErrorNewestVersion

            if curVersion == streamVersion
            {
                sec.raw = true
            }
        }

        // Read fat
        sig = stream.readNative'u32()
        if sig != Write.TagBinSignatures.Fat
            throw ErrorFat

        // Read fat
        fatCount := stream.readNative'uint()

        // When versions are the same, do not read the fat
        if sec.raw
        {
            stream.moveSeek(fatCount * (@sizeof(u32) + @sizeof(uint)))
        }
        else
        {
            while fatCount
            {
                crc  := stream.readNative'u32()
                seek := stream.readNative'uint()
                sec.mapSeek.add(crc, seek)
                fatCount -= 1
            }
        }

        return false
    }

    mtd endFields(fields: Array'(const *TypeValue)) throw
    {
        releaseSection()
    }

    mtd beginElement(val: TypeValue)->bool throw
    {
        reqCrcName := val.crc32

        // Outside a section
        if !sections.isEmpty()
        {
            sec := sections.back()

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
            throw ErrorCorrupted

        // Read type signature
        crcType := stream.readNative'u32()
        if crcType != val.pointedType.crc32
        {
            streamType := Reflection.crcToType(crcType)
            if !streamType
                throw ErrorBadType

            if !canConvertType(val.pointedType, streamType)
                throw ErrorBadType
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
            throw ErrorCorrupted

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

    mtd getVersion()->u32
    {
        if sections.isEmpty()
            return 0
        return sections.back().version
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