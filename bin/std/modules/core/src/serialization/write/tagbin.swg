#global namespace Serialization
#global namespace Write
#global public
using Swag

// Current tagbin version.
const TagBinVersion = 0xFAFAFAFA

// Signatures used to tag different sections and elements in the binary serialization format
enum TagBinSignatures: u32
{
    Root     = 0xC0DE0000     // Root element of the serialization structure
    Fat      = 0xC0DE0001     // FAT table identifier for unordered sections
    Sequence = 0xC0DE0002     // Start of a sequence (array or list)
    Version  = 0xC0DE0003     // Indicates that a version number is present
    Raw      = 0xC0DE0004     // Indicates raw POD data with no structure
    UnRaw    = 0xC0DE0005     // Indicates non-POD structured data
}

// Additional signature for each value, stored after the value description (crcs)
#[Swag.EnumFlags]
enum TagBinValueFlags: u8
{
    Zero
    Compressed
}

// Internal state for a binary serialization section
struct TagBinSection
{
    startSeek:     ConcatBufferSeek                      // Seek position in the output buffer where the section starts
    mapSeek:       HashTable'(u32, ConcatBufferSeek)     // Maps CRC keys to their position in the output buffer
    kind:          SectionKind                           // Type of section (e.g., Raw or Unordered)
}

// Binary writer for serialized data with support for structured and raw output
struct TagBin
{
    compressionFlags:        Core.Compress.Deflate.CompressionFlags = .Zero
    compressionLevel:        Core.Compress.Deflate.CompressionLevel = .BestSpeed
    compressionStrategy:     Core.Compress.Deflate.CompressionStrategy = .Default

    private
    {
        compressor:       Core.Compress.Deflate
        output:           *ConcatBuffer                                         // Output buffer where data is written
        freeSections:     ArrayPtr'TagBinSection                                // Pool of reusable section structures
        sections:         ArrayPtr'TagBinSection                                // Stack of currently active sections
        outStack:         Array'*ConcatBuffer
        elemStack:        Array'{ compressed: bool, tmpBuf: *ConcatBuffer }
        freeTmpBuf:       ArrayPtr'ConcatBuffer
    }
}

internal impl TagBin
{
    // Creates or reuses a new section for writing
    mtd newSection()->*TagBinSection
    {
        if .freeSections.count
        {
            let ptr = .freeSections.popBack()
            ptr.mapSeek.clear()
            .sections.add(ptr)
            return ptr
        }

        return .sections.addNewPtr()
    }

    // Releases the most recently added section and recycles it
    mtd releaseSection()
    {
        let ptr = .sections.popBack()
        .freeSections.add(ptr)
    }

    mtd acquireTmpBuf()->*ConcatBuffer
    {
        if .freeTmpBuf.count
        {
            let b = .freeTmpBuf.popBack()
            b.clear()
            return b
        }

        return .freeTmpBuf.newPtr()
    }

    mtd releaseTmpBuf(b: *ConcatBuffer)
    {
        b.clear()
        .freeTmpBuf.add(b)
    }
}

impl TagBin
{
    // Always returns true; indicates that output is textual (overridden stub)
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd isTextual()->bool
    {
        return true
    }

    // Begins writing to a specified output buffer
    mtd startWrite(buf: *ConcatBuffer)
    {
        .output = buf
        .outStack.clear()
        .elemStack.clear()
        .sections.clear()
    }

    // Begins the root section of the serialization structure
    mtd beginRoot()
    {
        .output.addNative(TagBinSignatures.Root)
        .output.addNative(TagBinVersion)
    }

    // Ends the root section and asserts that all subsections have been closed
    mtd endRoot()
    {
        Debug.assert(.sections.isEmpty())
    }

    // Begins a new section of the specified kind
    mtd beginSection(kind: SectionKind)
    {
        let sec = .newSection()
        sec.kind = kind

        if sec.kind == .Unordered
        {
            .output.addNative(TagBinSignatures.Fat)
            sec.startSeek = .output.getSeek()
            .output.addNative(0'u64) // Will store the fat offset here
            .output.addNative(0'u64) // Will store the fat count here
        }
    }

    // Ends the current section and writes out any required metadata
    mtd endSection()
    {
        let sec = .sections.back()

        if sec.kind == .Unordered
        {
            // Patch fat table seek and count
            let curSeek = .output.getSeek()
            .output.setSeek(sec.startSeek)
            .output.addNative(ConcatBuffer.getOffset(curSeek))
            .output.addNative(sec.mapSeek.count)
            .output.setSeek(curSeek)

            // Write fat table
            .output.addNative(TagBinSignatures.Fat)
            foreach k, v in sec.mapSeek
            {
                .output.addNative(k) // Crc
                .output.addNative(ConcatBuffer.getOffset(v)) // Linear seek inside the buffer
            }
        }

        .releaseSection()
    }

    // Begins a new struct, writing versioning and section metadata
    mtd beginStruct(structType: typeinfo, _structData: const *u8)->bool
    {
        let (pod, ?) = isPodFinal(structType)
        if pod
        {
            .output.addNative(TagBinSignatures.Raw)
            .beginSection(.Raw)
        }
        else
        {
            // Struct version
            .output.addNative(TagBinSignatures.Version)
            let version = Reflection.getAttributeValue(structType, Serialization.Version, "v")
            if version == null do
                .output.addNative(0'u32)
            else do
                .output.addNative(cast(u32) version)

            // Start unordered section
            .beginSection(.Unordered)
        }

        return false
    }

    // Ends the current struct section
    mtd endStruct()
    {
        .endSection()
    }

    // Begins a new element inside the current section and records its metadata
    mtd beginElement(val: TypeValue)
    {
        Debug.assert(.sections.count != 0, "Missing a previous 'beginSection' call")

        // If section is raw, then we just have values without versioning
        let sec = .sections.back()
        if sec.kind == .Raw do
            return

        // Register the current seek for the fat generation
        if sec.kind == .Unordered
        {
            Debug.assert(!sec.mapSeek.contains(val.crc))
            sec.mapSeek.add(val.crc, .output.getSeek())
        }

        // Write element signature
        .output.addNative(val.crc)
        .output.addNative(val.pointedType.crc)

        var valSig: TagBinValueFlags = .Zero
        if Reflection.hasAttribute(val, Compress) or Reflection.hasAttribute(val.pointedType, Compress) do
            valSig.add(TagBinValueFlags.Compressed)
        .output.addNative(valSig)

        // If compressed, push a new ConcatBuffer that will collect all payload for this element
        if valSig.has(.Compressed)
        {
            // push parent buffer on stack and switch current output
            let tmp = .acquireTmpBuf()
            .elemStack.add({compressed: true, tmpBuf: tmp})

            // From now, write to this temporary output
            .outStack.add(.output)
            .output = tmp
        }
        else
        {
            // track a non-compressed element to keep nesting consistent
            .elemStack.add({false, null})
        }
    }

    // Ends the current element (stubbed out)
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endElement(val: TypeValue)
    {
        // If section is raw, then we just have values without versioning
        let sec = .sections.back()
        if sec.kind == .Raw do
            return

        Debug.assert(.elemStack.count != 0)
        let ctx = .elemStack.popBack()
        if !ctx.compressed do
            return

        // We must have a previous buffer to append into
        Debug.assert(.outStack.count != 0)
        let parentOut = .outStack.popBack()

        // Gather uncompressed bytes
        let childBytes = ctx.tmpBuf.toString()

        // Compress
        .compressor.init(.compressionLevel, .compressionStrategy, .compressionFlags)

        var compressBuffer: Array'u8
        catch .compressor.compress(&compressBuffer, childBytes.toSlice())

        if compressBuffer.count >= childBytes.length or @err != null
        {
            // No compression, just store input
            parentOut.addNative(childBytes.length) // uncompressedSize
            parentOut.addNative(childBytes.length) // compressedSize
            parentOut.addBytes(childBytes)
        }
        else
        {
            parentOut.addNative(childBytes.length) // uncompressedSize
            parentOut.addNative(compressBuffer.count) // compressedSize
            parentOut.addBytes(compressBuffer.toSlice())
        }

        // Switch current 'output' back to parent and recycle tmp
        .output = parentOut
        .releaseTmpBuf(ctx.tmpBuf)
    }

    // Begins a value (stubbed out)
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd beginValue() {}

    // Ends a value (stubbed out)
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endValue() {}

    // Begins a sequence and determines if elements can be written in bulk as raw data
    mtd beginSequence(typeElem: typeinfo, data: const *void, countElem: u64)->bool
    {
        .output.addNative(TagBinSignatures.Sequence)
        .output.addNative(countElem)

        // Dynamic array of raw serializable types
        // Just write them in one batch
        if Reflection.isStructArray(typeElem)
        {
            let valueType = Reflection.getStructArrayType(typeElem)
            let (raw, sizeof) = isPodFinal(valueType)

            if raw
            {
                .output.addNative(TagBinSignatures.Raw)
                let bufArr = cast(const *Array'u8) data
                .output.addBytes(@mkslice(bufArr.buffer, countElem * sizeof))
                return true
            }
            else
            {
                .output.addNative(TagBinSignatures.UnRaw)
            }
        }

        // Static array of raw serializable types
        else
        {
            let (raw, sizeof) = isPodFinal(typeElem)
            if raw
            {
                .output.addNative(TagBinSignatures.Raw)
                .output.addBytes(@mkslice(cast(const [*] u8) data, countElem * sizeof))
                return true
            }
            else
            {
                .output.addNative(TagBinSignatures.UnRaw)
            }
        }

        return false
    }

    // Ends a sequence (stubbed out)
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endSequence() {}

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    // Writes a native value to the output buffer, with special handling for strings
    mtd(NT) writeNative(value: NT)
    {
        #if NT == string
        {
            .output.addNative(cast(u32) @countof(value))
            .output.addBytes(value)
        }
        #else
        {
            .output.addNative(value)
        }
    }
}
