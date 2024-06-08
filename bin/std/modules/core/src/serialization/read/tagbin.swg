#global namespace Serialization
#global namespace Read
#global public
using Swag, Core.Errors

/*
Binary serializer with forward/backward compatibility.
Changes that are supported from V to V+1 are :

* Add a new field. The new field will stay at the default value.
* Remove a field. It will just be ignored.
* Reorder of fields, as long as their name/type are the same.
* Rename a field with 'Serialization.Alias'.

Supported type changes are :

* Conversion from integer/float/rune/bool to integer/float/rune/bool.
* Change the size of a static array.
* Conversion from a static array to a dynamic `Core.Array`, and vice versa.
* Conversion from a single value to a static array or `Core.Array`, and vice versa.

Supported attributes are :

* 'Serialisation.Version'. If version is specified, then loading will not have versioning if versions are the same
* 'Serialization.Alias'. To rename a field. Multiple aliases are supported.
* 'Serialization.Final'. The struct will be loaded/saved without versioning (faster and lower serialization size)
* 'Serialization.PodFinal'. The struct will be loaded/saved in place without versioning
*/
struct TagBin
{
    stream:                ByteStream
    options:               TagBinOptions
    freeSections:          ArrayPtr'TagBinSection
    sections:              ArrayPtr'TagBinSection
    convertNextNative:     const *Swag.TypeInfoNative
    convertValToArray:     bool
    convertArrayToVal:     bool
}

struct TagBinSection
{
    mapSeek:          HashTable'(u32, u64)
    afterFatSeek:     u64
    version:          u32
    kind:             SectionKind
    skipFat:          bool
}

struct TagBinOptions
{
    tryForward: bool // Accept to try to read a newest version, otherwise error
}

internal impl TagBin
{
    mtd newSection()->*TagBinSection
    {
        if freeSections.count
        {
            let ptr = freeSections.popBack()
            ptr.mapSeek.clear()
            ptr.version = 0
            sections.add(ptr)
            return ptr
        }

        return sections.addNewPtr()
    }

    mtd releaseSection()
    {
        let ptr = sections.popBack()
        freeSections.add(ptr)
    }

    mtd canConvertType(toType, fromType: typeinfo)->bool
    {
        if toType == fromType do
            return true

        var fromStruct: const *Swag.TypeInfoStruct
        var toStruct:   const *Swag.TypeInfoStruct

        if Reflection.isStruct(fromType) do
            fromStruct = cast(const *Swag.TypeInfoStruct) fromType
        if Reflection.isStruct(toType) do
            toStruct = cast(const *Swag.TypeInfoStruct) toType

        // s64/float/bool/rune to s64/float/bool/rune
        if Reflection.isSimpleNative(toType) and Reflection.isSimpleNative(fromType)
        {
            convertNextNative = cast(const *Swag.TypeInfoNative) fromType
            return true
        }

        if Reflection.isStaticArray(fromType)
        {
            let fromArr = cast(const *Swag.TypeInfoArray) fromType

            // Array to array, this is fine, we will deal with size change
            if Reflection.isStaticArray(toType)
            {
                let toArr = cast(const *Swag.TypeInfoArray) toType
                if canConvertType(toArr.pointedType, fromArr.pointedType) do
                    return true
            }

            elif toStruct
            {
                // Static array to dynamic array
                if Reflection.isStructArray(toStruct)
                {
                    if canConvertType(Reflection.getStructArrayType(toStruct), fromArr.pointedType) do
                        return true
                }

                // Static array to single struct value
                elif canConvertType(toType, fromArr.pointedType)
                {
                    convertArrayToVal = true
                    return true
                }
            }

            // Static array to single value
            elif canConvertType(toType, fromArr.pointedType)
            {
                convertArrayToVal = true
                return true
            }
        }
        elif fromStruct
        {
            // Dynamic array to something...
            if fromStruct.fromGeneric == Core.Array
            {
                // Dynamic array to Static array
                if Reflection.isStaticArray(toType)
                {
                    let toArr = cast(const *Swag.TypeInfoArray) toType
                    if canConvertType(toArr.pointedType, Reflection.getStructArrayType(fromStruct)) do
                        return true
                }

                // Dynamic array to dynamic array
                elif toStruct and toStruct.fromGeneric == Core.Array
                {
                    if canConvertType(Reflection.getStructArrayType(toStruct), Reflection.getStructArrayType(fromStruct)) do
                        return true
                }

                // Dynamic array to single value
                elif canConvertType(toType, Reflection.getStructArrayType(fromStruct))
                {
                    convertArrayToVal = true
                    return true
                }
            }
        }

        if Reflection.isStaticArray(toType)
        {
            let toArr = cast(const *Swag.TypeInfoArray) toType

            // Conversion from simple value to array
            if canConvertType(fromType, toArr.pointedType)
            {
                convertValToArray = true
                return true
            }
        }
        elif toStruct
        {
            // To dynamic array...
            if Reflection.isStructArray(toStruct)
            {
                // Conversion from simple value to Core.Array
                if (!fromStruct or !Reflection.isStructArray(fromStruct)) and canConvertType(fromType, Reflection.getStructArrayType(toStruct))
                {
                    convertValToArray = true
                    return true
                }
            }

            // Struct to struct...
            elif fromStruct and !Reflection.isStructArray(fromStruct)
            {
                // 2 pod structs with the same size
                if Reflection.hasAttribute(toType, Serialization.PodFinal) and
                   Reflection.hasAttribute(fromType, Serialization.PodFinal) and
                   toType.sizeof == fromType.sizeof
                {
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
        let sig = stream.readNative'u32()
        if sig != Write.TagBinSignatures.Root do
            throw SerializationError{"invalid root signature"}
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endRoot() throw
    {
    }

    mtd beginSection(kind: SectionKind, skipFat = false) throw
    {
        let sec = newSection()
        sec.kind    = kind
        sec.skipFat = skipFat
        if kind != .Unordered do
            return

        // Read fat position and count
        var sig = stream.readNative'u32()
        if sig != Write.TagBinSignatures.Fat do
            throw SerializationError{"invalid fat signature"}
        let fatSeek  = stream.readNative'u64()
        var fatCount = stream.readNative'u64()

        // We have a fat, but we do not want it !
        if skipFat
        {
            sec.afterFatSeek = fatSeek
            sec.afterFatSeek += @sizeof(u32)
            // Signature
            sec.afterFatSeek += fatCount * @sizeof(u32)
            // Crc
            sec.afterFatSeek += fatCount * @sizeof(u64)
        }
        else
        {
            // Seek to fat and read it
            stream.setSeek(fatSeek)
            sig = stream.readNative'u32()
            if sig != Write.TagBinSignatures.Fat do
                throw SerializationError{"invalid fat signature"}

            while fatCount
            {
                let crc  = stream.readNative'u32()
                let seek = stream.readNative'u64()
                sec.mapSeek.add(crc, seek)
                fatCount -= 1
            }

            sec.afterFatSeek = stream.getSeek()
        }
    }

    mtd endSection() throw
    {
        let sec = sections.back()

        // Fat is after the datas. We need to be sure to point
        // to what's next, except in raw mode where we are already at
        // the correct offset
        if sec.kind != .Raw do
            stream.setSeek(sec.afterFatSeek)

        releaseSection()
    }

    mtd beginStruct(structType: typeinfo, _structData: const *u8)->bool throw
    {
        let sig = stream.readNative'u32()
        if sig == Write.TagBinSignatures.Raw
        {
            if !Reflection.hasAttribute(structType, Serialization.Final) and
               !Reflection.hasAttribute(structType, Serialization.PodFinal) do
                throw SerializationError{"cannot convert a type from final to non final"}

            beginSection(.Raw)
        }
        else
        {
            // Read version
            if sig != Write.TagBinSignatures.Version do
                throw SerializationError{"invalid version signature"}
            let streamVersion = stream.readNative'u32()

            // Deal with version
            var skipFat = false
            let version = Reflection.getAttributeValue(structType, Serialization.Version, "v")
            if version != null
            {
                let curVersion = cast(u32) version

                // The stream version is more recent than the code that tries to read it.
                // Specific error, except if we want to try forward compatibility
                if !options.tryForward and curVersion < streamVersion do
                    throw InvalidVersion{wantedVersion: curVersion, foundVersion: streamVersion}

                // If this is the exact same version, then no need to read the fat, because everything
                // is supposed to be in the correct order
                if curVersion == streamVersion do
                    skipFat = true
            }

            beginSection(.Unordered, skipFat)
        }

        return false
    }

    mtd endStruct() throw
    {
        endSection()
    }

    mtd beginElement(val: TypeValue)->bool throw
    {
        Debug.assert(sections.count != 0, "Missing a previous 'beginSection' call")

        var reqCrcName = val.crc32
        let sec        = sections.back()

        // If section is raw, then we just have values without versionning
        if sec.kind == .Raw do
            return true

        // If section is ordered, then we do not have a fat, but just elements
        // descriptors followed by values
        if sec.kind != .Ordered and !sec.skipFat
        {
            var ent = sec.mapSeek.find(val.crc32)

            // If ent is null, then we are requesting a field that does not exist
            // in the stream
            if !ent
            {
                // So check for aliases
                var aliases = Reflection.getAttributes(val, Serialization.Alias)
                visit al: aliases
                {
                    let value = Reflection.getAttributeValue(al, "name")
                    Debug.assert(value != null)
                    reqCrcName = Hash.Crc32.compute(cast(string) value)
                    ent        = sec.mapSeek.find(reqCrcName)
                    if ent do
                        break
                }
            }

            // The requested field is not there, just ignore it
            if !ent do
                return false

            // Otherwise we seek to the field in the stream
            stream.setSeek(ent.value)
        }

        // Read te name signature. We should have the crc
        let crcName = stream.readNative'u32()
        if crcName != reqCrcName do
            throw SerializationError{"bad field name", field: val}

        // Read type signature.
        // If they match, we are done. The value will be right after this.
        let crcType = stream.readNative'u32()
        if crcType == val.pointedType.crc32 do
            return true

        // We try to find a registered type with the crc from the stream.
        let streamType = Reflection.crcToType(crcType)

        // The type from the stream does not exists anymore...
        if !streamType do
            throw SerializationError{"unknown field type", field: val}

        // The type exists, but we do not have a way to convert it to the requested
        // type.
        if !canConvertType(val.pointedType, streamType) do
            throw SerializationError{"cannot convert field type", field: val}

        // It's an array in the stream, but now we want a single value.
        // Skip the sequence header
        if convertArrayToVal
        {
            convertArrayToVal = false
            let sig = stream.readNative'u32()
            if sig != Write.TagBinSignatures.Sequence do
                throw SerializationError{"stream is corrupted", field: val}
            discard stream.readNative'u64()
        }

        return true
    }

    mtd endElement() throw
    {
        convertNextNative = null
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd beginValue() throw
    {
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endValue() throw
    {
    }

    mtd beginSequence(typeElem: typeinfo, data: *u8, countElem: *u64)->bool throw
    {
        let orgCount = dref countElem
        if convertValToArray
        {
            dref countElem = 1
        }
        else
        {
            let sig = stream.readNative'u32()
            if sig != Write.TagBinSignatures.Sequence do
                throw SerializationError{"stream is corrupted"}

            // 0 for a dynamic array, > 0 for a static array
            dref countElem = stream.readNative'u64()
        }

        // If we need a value conversion, we must read element by element
        if convertValToArray
        {
            convertValToArray = false
            return false
        }

        // If we need a type conversion, we must read element by element
        if convertNextNative
        {
            let sig = stream.readNative'u32()
            if sig != Write.TagBinSignatures.Raw and sig != Write.TagBinSignatures.UnRaw do
                throw SerializationError{"stream is corrupted"}

            return false
        }

        // Dynamic array of raw serializable types
        // Just read them in one batch
        if Reflection.isStructArray(typeElem)
        {
            let valueType = Reflection.getStructArrayType(typeElem)
            let (raw, sizeof) = isPodFinal(valueType)

            let sig = stream.readNative'u32()
            if sig != Write.TagBinSignatures.Raw and sig != Write.TagBinSignatures.UnRaw do
                throw SerializationError{"stream is corrupted"}

            if raw
            {
                let bufArr = cast(*Array'u8) data
                bufArr.count = dref countElem
                bufArr.reserve(bufArr.count * sizeof)
                bufArr.capacity = bufArr.count
                stream.readBytes(bufArr.buffer, bufArr.count * sizeof)
                return true
            }
        }

        // Static array of raw serializable types
        else
        {
            dref countElem = Math.min(orgCount, dref countElem)
            let (raw, sizeof) = isPodFinal(typeElem)

            let sig = stream.readNative'u32()
            if sig != Write.TagBinSignatures.Raw and sig != Write.TagBinSignatures.UnRaw do
                throw SerializationError{"stream is corrupted"}

            if raw
            {
                stream.readBytes(data, dref countElem * sizeof)
                return true
            }
        }

        return false
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endSequence() throw
    {
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd toNextSequenceElement(countElem: *u64)->bool
    {
        if dref countElem == 0 do
            return false
        dref countElem -= 1
        return true
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd isTextual()->bool
    {
        return false
    }

    mtd getVersion()->u32
    {
        if sections.isEmpty() do
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
            let count   = cast(u64) stream.readNative'u32()
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
                    let v = stream.readNative'bool()
                    return cast(NT) v
                case U8:
                    let v = stream.readNative'u8()
                    return cast(NT) v
                case U16:
                    let v = stream.readNative'u16()
                    return cast(NT) v
                case U32, Rune:
                    let v = stream.readNative'u32()
                    return cast(NT) v
                case U64:
                    let v = stream.readNative'u64()
                    return cast(NT) v
                case S8:
                    let v = stream.readNative's8()
                    return cast(NT) v
                case S16:
                    let v = stream.readNative's16()
                    return cast(NT) v
                case S32:
                    let v = stream.readNative's32()
                    return cast(NT) v
                case S64:
                    let v = stream.readNative's64()
                    return cast(NT) v
                case F32:
                    let v = stream.readNative'f32()
                    return cast(NT) v
                case F64:
                    let v = stream.readNative'f64()
                    return cast(NT) v
                }
            }

            return stream.readNative'NT()
        }
    }
}
