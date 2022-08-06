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
    - Conversion from integer/float/rune/bool to integer/float/rune/bool
    - Conversion from single value to array/Core.Array, and vice versa

    Supported attributes are :

    - Serialisation.Version     If version is specified, then loading will not have versioning if versions are the same
    - Serialization.Alias       To rename a field. Multiple aliases are supported.
    - Serialization.Final       The struct will be loaded/saved without versioning (faster and lower serialization size)
    - Serialization.PodFinal    The struct will be loaded/saved in place without versioning
*/

struct TagBinSection
{
    mapSeek:        HashTable'(u32, uint)
    afterFatSeek:   uint
    version:        u32
    kind:           SectionKind
    skipFat:        bool
}

struct TagBinOptions
{
    tryForward: bool    // Accept to try to read a newest version, otherwise error
}

struct TagBin
{
    stream:             ByteStream
    options:            TagBinOptions
    freeSections:       ArrayPtr'TagBinSection
    sections:           ArrayPtr'TagBinSection
    convertNextNative:  const *Swag.TypeInfoNative
    convertValToArray:  bool
    convertArrayToVal:  bool
}

impl TagBin
{
    const ErrorInvalidFile      = "invalid file"
    const ErrorCorrupted        = "stream is corrupted"
    const ErrorFat              = "invalid fat signature"
    const ErrorNewestVersion    = "version is too recent"
    const ErrorBadName          = "bad field name"
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

    mtd canConvertType(to, from: typeinfo)->bool
    {
        if to == from
            return true

        // int/float/bool/rune to int/float/bool/rune
        if Reflection.isSimpleNative(to) and Reflection.isSimpleNative(from)
        {
            convertNextNative = cast(const *Swag.TypeInfoNative) from
            return true
        }

        if Reflection.isArray(from)
        {
            fromArr := cast(const *Swag.TypeInfoArray) from

            // Array to array, this is fine, we will deal with size change
            if Reflection.isArray(to)
            {
                toArr := cast(const *Swag.TypeInfoArray) to
                if canConvertType(toArr.pointedType, fromArr.pointedType)
                    return true
            }

            elif Reflection.isStruct(to)
            {
                toStruct := cast(const *Swag.TypeInfoStruct) to

                // Static array to dynamic array
                if toStruct.fromGeneric == @typeof(Array)
                {
                    if canConvertType(toStruct.generics[0].pointedType, fromArr.pointedType)
                        return true
                }

                // Static array to single struct value
                elif canConvertType(to, fromArr.pointedType)
                {
                    convertArrayToVal = true
                    return true
                }
            }

            // Static array to single value
            elif canConvertType(to, fromArr.pointedType)
            {
                convertArrayToVal = true
                return true
            }
        }
        elif Reflection.isStruct(from)
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

                // Dynamic array to single value
                elif canConvertType(to, fromStruct.generics[0].pointedType)
                {
                    convertArrayToVal = true
                    return true
                }
            }
        }

        if Reflection.isArray(to)
        {
            toArr := cast(const *Swag.TypeInfoArray) to

            // Conversion from simple value to array
            if canConvertType(from, toArr.pointedType)
            {
                convertValToArray = true
                return true
            }
        }
        elif Reflection.isStruct(to)
        {
            toStruct := cast(const *Swag.TypeInfoStruct) to
            if toStruct.fromGeneric == @typeof(Array)
            {
                // Conversion from simple value to Core.Array
                if canConvertType(from, toStruct.generics[0].pointedType)
                {
                    convertValToArray = true
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

    mtd beginSection(kind: SectionKind, skipFat = false) throw
    {
        sec := newSection()
        sec.kind    = kind
        sec.skipFat = skipFat
        if kind != .Unordered
            return

        // Read fat position and count
        sig := stream.readNative'u32()
        if sig != Write.TagBinSignatures.Fat
            throw ErrorFat
        fatSeek  := stream.readNative'uint()
        fatCount := stream.readNative'uint()

        // We have a fat, but we do not want it !
        if skipFat
        {
            sec.afterFatSeek = fatSeek
            sec.afterFatSeek += @sizeof(u32) // Signature
            sec.afterFatSeek += fatCount * @sizeof(u32) // Crc
            sec.afterFatSeek += fatCount * @sizeof(uint) // Element seek
        }
        else
        {
            // Seek to fat and read it
            stream.setSeek(fatSeek)
            sig = stream.readNative'u32()
            if sig != Write.TagBinSignatures.Fat
                throw ErrorFat

            while fatCount
            {
                crc  := stream.readNative'u32()
                seek := stream.readNative'uint()
                sec.mapSeek.add(crc, seek)
                fatCount -= 1
            }

            sec.afterFatSeek = stream.getSeek()
        }
    }

    mtd endSection() throw
    {
        sec := sections.back()

        // Fat is after the datas. We need to be sure to point
        // to what's next, except in raw mode where we are already at
        // the correct offset
        if sec.kind != .Raw
            stream.setSeek(sec.afterFatSeek)

        releaseSection()
    }

    mtd beginFields(structType: typeinfo, structData: const *u8)->bool throw
    {
        if  Reflection.hasAttribute(structType, Serialization.Final) or
            Reflection.hasAttribute(structType, Serialization.PodFinal)
        {
            beginSection(.Raw)
        }
        else
        {
            // Read version
            sig := stream.readNative'u32()
            if sig != Write.TagBinSignatures.Version
                throw ErrorCorrupted
            streamVersion := stream.readNative'u32()

            // Deal with version
            skipFat := false
            version := Reflection.getAttributeValue(structType, Serialization.Version, "v")
            if version != null
            {
                curVersion := cast(u32) version

                // Stream version is more recent than current code.
                // Error, except if we want to try forward compatibility
                if !options.tryForward and curVersion < streamVersion
                    throw ErrorNewestVersion

                // If this is the exact same version, then no need to read the fat
                if curVersion == streamVersion
                    skipFat = true
            }

            beginSection(.Unordered, skipFat)
        }

        return false
    }

    mtd endFields() throw
    {
        endSection()
    }

    mtd beginElement(val: TypeValue)->bool throw
    {
        reqCrcName := val.crc32
        sec := sections.back()

        // If section is raw, then we just have values without versionning
        if sec.kind == .Raw
            return true

        // If section is ordered, then we do not have a fat, but just elements
        // descriptors followed by values
        if sec.kind != .Ordered and !sec.skipFat
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

            // No alias, the field is not there, just ignore it
            if !ent
                return false

            stream.setSeek(ent.value)
        }

        // Read name signature
        crcName := stream.readNative'u32()
        if crcName != reqCrcName
            throw ErrorBadName

        // Read type signature. If they match, we are done
        crcType := stream.readNative'u32()
        if crcType == val.pointedType.crc32
            return true

        // Type from the stream is different
        streamType := Reflection.crcToType(crcType)
        if !streamType
            throw ErrorBadType
        if !canConvertType(val.pointedType, streamType)
            throw ErrorBadType

        // It's an array in the stream, and we do not want one
        // Skip header
        if convertArrayToVal
        {
            convertArrayToVal = false
            sig := stream.readNative'u32()
            if sig != Write.TagBinSignatures.Sequence
                throw ErrorCorrupted
            discard stream.readNative'uint()
        }

        return true
    }

    mtd endElement() throw
    {
        convertNextNative = null
    }

    mtd beginValue() throw
    {
    }

    mtd endValue() throw
    {
    }

    mtd beginSequence(typeElem: typeinfo, data: *u8, countElem: *uint)->bool throw
    {
        orgCount := dref countElem

        // It's not an array in the stream, but we want one
        if convertValToArray
        {
            convertValToArray = false
            dref countElem = 1
        }
        else
        {
            sig := stream.readNative'u32()

            if sig != Write.TagBinSignatures.Sequence
                throw ErrorCorrupted

            // 0 for a dynamic array, > 0 for a static array
            dref countElem = stream.readNative'uint()
        }

        // If we need to type conversion, we must read element by element
        if convertNextNative
            return false

        // Dynamic array of raw serializable types
        // Just read them in one batch
        if typeElem.kind == .Struct
        {
            structType := cast(const *TypeInfoStruct) typeElem
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType
                (raw, sizeof) := isPodFinal(valueType)

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

            (raw, sizeof) := isPodFinal(typeElem)
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
            if convertNextNative
            {
                #[Swag.Safety("", false)]
                switch convertNextNative.nativeKind
                {
                case Bool:
                    v := stream.readNative'bool()
                    return cast(NT) v
                case U8:
                    v := stream.readNative'u8()
                    return cast(NT) v
                case U16:
                    v := stream.readNative'u16()
                    return cast(NT) v
                case U32, Rune:
                    v := stream.readNative'u32()
                    return cast(NT) v
                case U64, UInt:
                    v := stream.readNative'u64()
                    return cast(NT) v
                case S8:
                    v := stream.readNative's8()
                    return cast(NT) v
                case S16:
                    v := stream.readNative's16()
                    return cast(NT) v
                case S32:
                    v := stream.readNative's32()
                    return cast(NT) v
                case S64, Int:
                    v := stream.readNative's64()
                    return cast(NT) v
                case F32:
                    v := stream.readNative'f32()
                    return cast(NT) v
                case F64:
                    v := stream.readNative'f64()
                    return cast(NT) v
                }
            }

            return stream.readNative'NT()
        }
    }
}