#global namespace Serialization
#global namespace Read
#global public
using Swag, Core.Errors

alias Buffer = Array'u8

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
* Rename a struct with 'Serialization.Alias'.
* Convert an existing complexe type to 'Serialization.Final'

Supported attributes are :

* 'Serialisation.Version'. If version is specified, then loading will not have versioning if versions are the same
* 'Serialization.Alias'. To rename a field or a struct type. Multiple aliases are supported.
* 'Serialization.Final'. The struct will be loaded/saved without versioning (faster and lower serialization size)
*/
struct TagBin
{
    version:               u32                                             // Version of the tagbin in the stream
    ignoreElements:        Array'String                                    // A list of element names that should be skipped
    stream:                ByteStream                                      // Input byte stream for reading serialized data
    options:               TagBinOptions                                   // Configuration options for deserialization behavior
    freeSections:          ArrayPtr'TagBinSection                          // Pool of reusable section objects for memory efficiency
    sections:              ArrayPtr'TagBinSection                          // Stack of active sections during deserialization
    convertNextNative:     const *Swag.TypeInfoNative                      // Type information for pending native type conversion
    convertValToArray:     bool                                            // Flag indicating next value should be converted to array
    convertArrayToVal:     bool                                            // Flag indicating next array should be converted to single value

    outStack:              Array'{ buf: const [..] u8, seek: u64 }
    elemStack:             Array'{ compressed: bool, tmpBuf: *Buffer }
    tmpPool:               ArrayPtr'Buffer
}

// Section information for tracking deserialization state within structured data.
struct TagBinSection
{
    // Map from field name CRC to stream position
    mapSeek:          HashTable'(u32, u64)
    afterFatSeek:     u64                      // Stream position after the section's fat table
    version:          u32                      // Version number of this section
    kind:             SectionKind              // Type of section (ordered, unordered, or raw)
    skipFat:          bool                     // Whether to skip reading the fat table for this section
}

// Configuration options for controlling deserialization behavior.
struct TagBinOptions
{
    tryForward: bool // Accept to try to read a newest version, otherwise error
}

internal impl TagBin
{
    // Create a new section for the deserialization stack.
    // Returns a pointer to a section object, either reused from the free pool or newly allocated.
    mtd newSection()->*TagBinSection
    {
        if .freeSections.count
        {
            let ptr = .freeSections.popBack()
            ptr.mapSeek.clear()
            ptr.version = 0
            .sections.add(ptr)
            return ptr
        }

        return .sections.addNewPtr()
    }

    // Release the current section back to the free pool.
    // Pops the most recent section from the active stack and returns it to the reuse pool.
    mtd releaseSection()
    {
        let ptr = .sections.popBack()
        .freeSections.add(ptr)
    }

    mtd acquireTmpBuf()->*Buffer
    {
        if .tmpPool.count
        {
            let b = .tmpPool.popBack()
            b.clear()
            return b
        }

        return .tmpPool.newPtr()
    }

    mtd releaseTmpBuf(b: *Buffer)
    {
        b.clear()
        .tmpPool.add(b)
    }

    // Check if type conversion is possible between 'fromType' and 'toType'.
    // Sets up conversion flags and returns true if the conversion is supported.
    mtd canConvertType(toType, fromType: #null typeinfo)->bool
    {
        if toType == fromType do
            return true
        if !toType or !fromType do
            return false

        var fromStruct: const *Swag.TypeInfoStruct
        var toStruct:   const *Swag.TypeInfoStruct

        if Reflection.isStruct(fromType) do
            fromStruct = cast(const *Swag.TypeInfoStruct) fromType
        if Reflection.isStruct(toType) do
            toStruct = cast(const *Swag.TypeInfoStruct) toType

        // s64/float/bool/rune to s64/float/bool/rune
        if Reflection.isSimpleNative(toType) and Reflection.isSimpleNative(fromType)
        {
            .convertNextNative = cast(const *Swag.TypeInfoNative) fromType
            return true
        }

        if Reflection.isStaticArray(fromType)
        {
            let fromArr = cast(const *Swag.TypeInfoArray) fromType

            // Array to array, this is fine, we will deal with size change
            if Reflection.isStaticArray(toType)
            {
                let toArr = cast(const *Swag.TypeInfoArray) toType
                if .canConvertType(toArr.pointedType, fromArr.pointedType) do
                    return true
            }

            elif toStruct
            {
                // Static array to dynamic array
                if Reflection.isStructArray(toStruct)
                {
                    if .canConvertType(Reflection.getStructArrayType(toStruct), fromArr.pointedType) do
                        return true
                }

                // Static array to single struct value
                elif .canConvertType(toType, fromArr.pointedType)
                {
                    .convertArrayToVal = true
                    return true
                }
            }

            // Static array to single value
            elif .canConvertType(toType, fromArr.pointedType)
            {
                .convertArrayToVal = true
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
                    if .canConvertType(toArr.pointedType, Reflection.getStructArrayType(fromStruct)) do
                        return true
                }

                // Dynamic array to dynamic array
                elif toStruct and toStruct.fromGeneric == Core.Array
                {
                    if .canConvertType(Reflection.getStructArrayType(toStruct), Reflection.getStructArrayType(fromStruct)) do
                        return true
                }

                // Dynamic array to single value
                elif .canConvertType(toType, Reflection.getStructArrayType(fromStruct))
                {
                    .convertArrayToVal = true
                    return true
                }
            }
        }

        if Reflection.isStaticArray(toType)
        {
            let toArr = cast(const *Swag.TypeInfoArray) toType

            // Conversion from simple value to array
            if .canConvertType(fromType, toArr.pointedType)
            {
                .convertValToArray = true
                return true
            }
        }
        elif toStruct
        {
            // To dynamic array...
            if Reflection.isStructArray(toStruct)
            {
                // Conversion from simple value to Core.Array
                if (!fromStruct or !Reflection.isStructArray(fromStruct)) and .canConvertType(fromType, Reflection.getStructArrayType(toStruct))
                {
                    .convertValToArray = true
                    return true
                }
            }

            // Struct to struct...
            elif fromStruct and !Reflection.isStructArray(fromStruct)
            {
                // Alias
                let aliases = Reflection.getAttributes(toType, Serialization.Alias)
                foreach al in aliases
                {
                    let value = Reflection.getAttributeValue(al, "name")
                    if cast(string) value == fromType.name
                    {
                        return true
                    }
                }
            }
        }

        return false
    }
}

impl TagBin
{
    // Initialize the deserializer with the given input byte array.
    // Sets up the internal byte stream for reading from the provided data buffer.
    mtd startRead(input: const [..] u8) throw
    {
        .stream = ByteStream.openRead(input)
    }

    // Begin reading the root container of the serialized data.
    // Validates the root signature to ensure the stream contains valid serialized data.
    mtd beginRoot() throw
    {
        let sig = .stream.readNative'u32()
        if sig != Write.TagBinSignatures.Root do
            throw SerializationError{"invalid root signature"}
        .version = .stream.readNative'u32()
    }

    // Finish reading the root container.
    // Currently no cleanup is required, but this maintains the symmetric begin/end API.
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endRoot() throw
    {
    }

    // Begin reading a new section with the specified 'kind'.
    // Optionally skips reading the fat table if 'skipFat' is true for performance optimization.
    mtd beginSection(kind: SectionKind, skipFat = false) throw
    {
        let sec = .newSection()
        sec.kind    = kind
        sec.skipFat = skipFat
        if kind != .Unordered do
            return

        // Read fat position and count
        var sig = .stream.readNative'u32()
        if sig != Write.TagBinSignatures.Fat do
            throw SerializationError{"invalid fat signature"}
        let fatSeek  = .stream.readNative'u64()
        var fatCount = .stream.readNative'u64()

        // We have a fat, but we do not want it !
        if skipFat
        {
            sec.afterFatSeek = fatSeek
            sec.afterFatSeek += #sizeof(u32)
            // Signature
            sec.afterFatSeek += fatCount * #sizeof(u32)
            // Crc
            sec.afterFatSeek += fatCount * #sizeof(u64)
        }
        else
        {
            // Seek to fat and read it
            .stream.setSeek(fatSeek)
            sig = .stream.readNative'u32()
            if sig != Write.TagBinSignatures.Fat do
                throw SerializationError{"invalid fat signature"}

            while fatCount
            {
                let crc  = .stream.readNative'u32()
                let seek = .stream.readNative'u64()
                sec.mapSeek.add(crc, seek)
                fatCount -= 1
            }

            sec.afterFatSeek = .stream.getSeek()
        }
    }

    // Finish reading the current section.
    // Updates the stream position and releases the section back to the pool.
    mtd endSection() throw
    {
        let sec = .sections.back()

        // Fat is after the datas. We need to be sure to point
        // to what's next, except in raw mode where we are already at
        // the correct offset
        if sec.kind != .Raw do
            .stream.setSeek(sec.afterFatSeek)

        .releaseSection()
    }

    // Begin reading a struct of the given 'structType'.
    // Returns false for normal struct processing, true for special handling cases.
    mtd beginStruct(structType: typeinfo, _structData: const *u8)->bool throw
    {
        var sig = .stream.readNative'u32()
        if sig == Write.TagBinSignatures.Raw
        {
            // If the stream contains a 'raw' representation of the struct, and the requested
            // type is not a POD, then we can load in place.
            if !Reflection.hasAttribute(structType, Serialization.Final) do
                throw SerializationError{"cannot convert a type from final to non final"}

            .beginSection(.Raw)
        }
        else
        {
            // Skip array in case of an array to value conversion
            if sig == Write.TagBinSignatures.UnRaw do
                sig = .stream.readNative'u32()

            // Read struct version
            if sig != Write.TagBinSignatures.Version do
                throw SerializationError{"invalid version signature"}
            let streamVersion = .stream.readNative'u32()

            // Deal with version
            var skipFat       = false
            let structVersion = Reflection.getAttributeValue(structType, Serialization.Version, "v")
            if structVersion != null
            {
                let curVersion = cast(u32) structVersion

                // The stream version is more recent than the code that tries to read it.
                // Specific error, except if we want to try forward compatibility
                if !.options.tryForward and curVersion < streamVersion do
                    throw InvalidVersion{wantedVersion: curVersion, foundVersion: streamVersion}

                // If this is the exact same version, then no need to read the fat, because everything
                // is supposed to be in the correct order
                if curVersion == streamVersion do
                    skipFat = true
            }

            .beginSection(.Unordered, skipFat)
        }

        return false
    }

    // Finish reading the current struct.
    // Closes the current section that was opened by 'beginStruct'.
    mtd endStruct() throw
    {
        .endSection()
    }

    // Begin reading a field element identified by 'val'.
    // Returns true if the element exists in the stream and should be processed, false to skip it.
    mtd beginElement(val: TypeValue)->bool throw
    {
        Debug.assert(.sections.count != 0, "Missing a previous 'beginSection' call")

        // If this name is in the ignore list, then skip it
        if .ignoreElements.contains(val.name)
        {
            .elemStack.add({false, null})
            return false
        }

        var reqCrcName = val.crc
        let sec        = .sections.back()

        // If section is raw, then we just have values without versionning
        if sec.kind == .Raw
        {
            .elemStack.add({false, null})
            return true
        }

        // If section is unordered, then we have a fat where each value has a descriptor
        if sec.kind == .Unordered and !sec.skipFat
        {
            var ent = sec.mapSeek.tryFind(val.crc)

            // If ent is null, then we are requesting a field that does not exist
            // in the stream
            if !ent
            {
                // So check for aliases
                let aliases = Reflection.getAttributes(val, Serialization.Alias)
                foreach al in aliases
                {
                    let value = Reflection.getAttributeValue(al, "name")
                    Debug.assert(value != null)
                    reqCrcName = Hash.Crc32.compute(cast(string) value)
                    ent        = sec.mapSeek.tryFind(reqCrcName)
                    if ent do
                        break
                }
            }

            // The requested field is not there, just ignore it
            if !ent
            {
                .elemStack.add({false, null})
                return false
            }

            // Otherwise we seek to the field in the stream
            .stream.setSeek(ent.value)
        }

        // Read the name signature.
        let crcName = .stream.readNative'u32()

        // We should have a matching crc
        if crcName != reqCrcName do
            throw SerializationError{"bad field name", field: val}

        // Read the type signature.
        let crcType = .stream.readNative'u32()

        // Read value flags
        let flags = .stream.readNative'Write.TagBinValueFlags()
        if flags.has(.Compressed)
        {
            let uncompressedSize = .stream.readNative'u64()
            let compressedSize   = .stream.readNative'u64()
            if uncompressedSize == compressedSize do
                .elemStack.add({false, null})
            else
            {
                // push parent buffer on stack and switch current output
                let decompressedBuffer = .acquireTmpBuf()
                decompressedBuffer.reserve(uncompressedSize)

                // Here uncompress to 'tmp'
                var decompressor: Core.Compress.Inflate
                decompressor.decompress(decompressedBuffer, .stream.remainReadToSlice())

                // track this element context
                .elemStack.add({true, decompressedBuffer})

                .outStack.add({.stream.readBuffer, .stream.seek + compressedSize})
                .stream.readBuffer = decompressedBuffer.toSlice()
                .stream.seek       = 0
            }
        }
        else
        {
            .elemStack.add({false, null})
        }

        // If they do not match, we will try to convert the type from the one in the
        // stream to the requested one
        if crcType != val.pointedType.crc
        {
            // We try to find a registered type with the crc from the stream.
            let streamType = Reflection.crcToType(crcType)

            // The type from the stream does not exists anymore...
            if !streamType do
                throw SerializationError{"unknown field type", field: val}

            // The type exists, but we do not have a way to convert it to the requested
            // type.
            if !.canConvertType(val.pointedType, streamType) do
                throw SerializationError{"cannot convert field type", field: val}

            // It's an array in the stream, but now we want a single value.
            // Skip the sequence header
            if .convertArrayToVal
            {
                .convertArrayToVal = false
                let sig = .stream.readNative'u32()
                if sig != Write.TagBinSignatures.Sequence do
                    throw SerializationError{"stream is corrupted", field: val}
                discard .stream.readNative'u64()
            }
        }

        return true
    }

    // Finish reading the current field element.
    // Clears any pending native type conversion state.
    mtd endElement(_val: TypeValue) throw
    {
        .convertNextNative = null

        Debug.assert(.elemStack.count != 0)
        let ctx = .elemStack.popBack()
        if !ctx.compressed do
            return

        // We must have a previous buffer to append into
        Debug.assert(.outStack.count != 0)
        let (parentOut, parentSeek) = .outStack.popBack()

        // Switch current 'output' back to parent and recycle tmp
        .stream.readBuffer = parentOut
        .stream.seek       = parentSeek
        .releaseTmpBuf(ctx.tmpBuf)
    }

    // Begin reading a value within the current context.
    // Currently no special processing is required, but this maintains the symmetric API.
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd beginValue() throw
    {
    }

    // Finish reading a value within the current context.
    // Currently no cleanup is required, but this maintains the symmetric API.
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endValue() throw
    {
    }

    // Begin reading a sequence (array) of elements of type 'typeElem'.
    // Updates 'countElem' with the actual number of elements and returns true for bulk reading optimization.
    mtd beginSequence(typeElem: typeinfo, data: *u8, countElem: *u64)->bool throw
    {
        let orgCount = dref countElem
        if .convertValToArray
        {
            dref countElem = 1
        }
        else
        {
            let sig = .stream.readNative'u32()
            if sig != Write.TagBinSignatures.Sequence do
                throw SerializationError{"stream is corrupted"}

            // 0 for a dynamic array, > 0 for a static array
            dref countElem = .stream.readNative'u64()
        }

        // If we need a value conversion, we must read element by element
        if .convertValToArray
        {
            .convertValToArray = false
            return false
        }

        // If we need a type conversion, we must read element by element
        if .convertNextNative
        {
            let sig = .stream.readNative'u32()
            if sig != Write.TagBinSignatures.Raw and sig != Write.TagBinSignatures.UnRaw do
                throw SerializationError{"stream is corrupted"}

            return false
        }

        // Dynamic array
        if Reflection.isStructArray(typeElem)
        {
            let valueType = Reflection.getStructArrayType(typeElem)
            let (raw, sizeof) = isPodFinal(valueType)

            let sig = .stream.readNative'u32()
            if sig != Write.TagBinSignatures.Raw and sig != Write.TagBinSignatures.UnRaw do
                throw SerializationError{"stream is corrupted"}

            // Dynamic array of raw serializable types
            // Just read them in one batch
            if raw
            {
                let bufArr = cast(*Array'u8) data
                bufArr.count = dref countElem
                bufArr.reserve(bufArr.count * sizeof)
                bufArr.capacity = bufArr.count
                .stream.readBytes(bufArr.buffer, bufArr.count * sizeof)
                return true
            }
        }

        // Static array
        else
        {
            dref countElem = Math.min(orgCount, dref countElem)
            let (raw, sizeof) = isPodFinal(typeElem)

            let sig = .stream.readNative'u32()
            if sig != Write.TagBinSignatures.Raw and sig != Write.TagBinSignatures.UnRaw do
                throw SerializationError{"stream is corrupted"}

            // Static array of raw serializable types
            if raw
            {
                .stream.readBytes(data, dref countElem * sizeof)
                return true
            }
        }

        return false
    }

    // Finish reading the current sequence.
    // Currently no cleanup is required, but this maintains the symmetric API.
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endSequence() throw
    {
    }

    // Advance to the next element in the current sequence.
    // Decrements 'countElem' and returns false when no more elements remain.
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd toNextSequenceElement(countElem: *u64)->bool
    {
        if dref countElem == 0 do
            return false
        dref countElem -= 1
        return true
    }

    // Check if this serializer uses textual representation.
    // Always returns false since this is a binary serializer.
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd isTextual()->bool
    {
        return false
    }

    // Get the version number of the current section.
    // Returns 0 if no sections are active, otherwise returns the version of the most recent section.
    mtd getVersion()->u32
    {
        if .sections.isEmpty() do
            return 0
        return .sections.back().version
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    // Read a native type value from the stream with automatic type conversion support.
    // Handles string types specially and performs type conversions when 'convertNextNative' is set.
    mtd(NT) readNative()->NT throw
    {
        #if NT == Core.String
        {
            var result: retval
            let count   = cast(u64) .stream.readNative'u32()
            result.reserve(count)
            .stream.readBytes(result.buffer, count)
            result.length = count
            return result
        }
        #else
        {
            if .convertNextNative
            {
                #[Swag.Safety(.All, .All, false)]
                switch .convertNextNative.nativeKind
                {
                case Bool:
                    let v = .stream.readNative'bool()
                    return cast(NT) v
                case U8:
                    let v = .stream.readNative'u8()
                    return cast(NT) v
                case U16:
                    let v = .stream.readNative'u16()
                    return cast(NT) v
                case U32, Rune:
                    let v = .stream.readNative'u32()
                    return cast(NT) v
                case U64:
                    let v = .stream.readNative'u64()
                    return cast(NT) v
                case S8:
                    let v = .stream.readNative's8()
                    return cast(NT) v
                case S16:
                    let v = .stream.readNative's16()
                    return cast(NT) v
                case S32:
                    let v = .stream.readNative's32()
                    return cast(NT) v
                case S64:
                    let v = .stream.readNative's64()
                    return cast(NT) v
                case F32:
                    let v = .stream.readNative'f32()
                    return cast(NT) v
                case F64:
                    let v = .stream.readNative'f64()
                    return cast(NT) v
                }
            }

            return .stream.readNative'NT()
        }
    }
}
