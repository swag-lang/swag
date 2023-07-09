#global namespace Serialization.Write
#global public
using Swag

enum TagBinSignatures: u32
{
    Root        = 0xC0DE0000
    Fat         = 0xC0DE0001
    Sequence    = 0xC0DE0002
    Version     = 0xC0DE0003
}

struct TagBinSection
{
    startSeek:  ConcatBufferSeek
    mapSeek:    HashTable'(u32, ConcatBufferSeek)
    kind:       SectionKind
}

struct TagBin
{
    output:         *ConcatBuffer
    freeSections:   ArrayPtr'TagBinSection
    sections:       ArrayPtr'TagBinSection
}

private impl TagBin
{
    mtd newSection()->*TagBinSection
    {
        if freeSections.count
        {
            var ptr = freeSections.popBack()
            ptr.mapSeek.clear()
            sections.add(ptr)
            return ptr
        }

        return sections.addNewPtr()
    }

    mtd releaseSection()
    {
        var ptr = sections.popBack()
        freeSections.add(ptr)
    }
}

impl TagBin
{
    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
    }

    mtd beginRoot()
    {
        output.addNative(TagBinSignatures.Root)
    }

    mtd endRoot()
    {
        Debug.assert(sections.isEmpty())
    }

    mtd beginSection(kind: SectionKind)
    {
        var sec = newSection()
        sec.kind = kind
        if sec.kind != .Unordered
            return

        output.addNative(TagBinSignatures.Fat)
        sec.startSeek = output.getSeek()
        output.addNative(0'u64) // Will store the fat offset here
        output.addNative(0'u64) // Will store the fat count here
    }

    mtd endSection()
    {
        var sec = sections.back()
        if sec.kind != .Unordered
        {
            releaseSection()
            return
        }

        // Patch fat table seek and count
        var curSeek = output.getSeek()
        output.setSeek(sec.startSeek)
        output.addNative(ConcatBuffer.getOffset(curSeek))
        output.addNative(sec.mapSeek.count)
        output.setSeek(curSeek)

        // Write fat table
        output.addNative(TagBinSignatures.Fat)
        visit k, v: sec.mapSeek
        {
            output.addNative(k) // Crc
            output.addNative(ConcatBuffer.getOffset(v)) // Linear seek inside the buffer
        }

        releaseSection()
    }

    mtd beginStruct(structType: typeinfo, _structData: const *u8)->bool
    {
        if  Reflection.hasAttribute(structType, Serialization.Final) or
            Reflection.hasAttribute(structType, Serialization.PodFinal)
        {
            beginSection(.Raw)
        }
        else
        {
            // Struct version
            output.addNative(TagBinSignatures.Version)
            var version = Reflection.getAttributeValue(structType, Serialization.Version, "v")
            if version == null
                output.addNative(0'u32)
            else
                output.addNative(cast(u32) version)

            // Start unordered section
            beginSection(.Unordered)
        }

        return false
    }

    mtd endStruct()
    {
        endSection()
    }

    mtd beginElement(val: TypeValue)
    {
        Debug.assert(sections.count != 0, "Missing a previous 'beginSection' call")

        var sec = sections.back()

        // If section is raw, then we just have values without versionning
        if sec.kind == .Raw
            return

        // Register the current seek for the fat generation
        if sec.kind == .Unordered
        {
            Debug.assert(!sec.mapSeek.contains(val.crc32))
            sec.mapSeek.add(val.crc32, output.getSeek())
        }

        // Write element signature
        output.addNative(val.crc32)
        output.addNative(val.pointedType.crc32)
    }

    #[Warn("wrn0005", WarnLevel.Disable)]
    mtd endElement()
    {
    }

    #[Warn("wrn0005", WarnLevel.Disable)]
    mtd beginValue()
    {
    }

    #[Warn("wrn0005", WarnLevel.Disable)]
    mtd endValue()
    {
    }

    mtd beginSequence(typeElem: typeinfo, data: const *void, countElem: u64)->bool
    {
        output.addNative(TagBinSignatures.Sequence)
        output.addNative(countElem)

        // Dynamic array of raw serializable types
        // Just write them in one batch
        if typeElem.kind == .Struct
        {
            var structType = cast(const *TypeInfoStruct) typeElem
            if structType.fromGeneric == @typeof(Array)
            {
                var valueType = structType.generics[0].pointedType
                var (raw, sizeof) = isPodFinal(valueType)

                if raw
                {
                    var bufArr = cast(const *Array'u8) data
                    output.addBytes(@mkslice(bufArr.buffer, countElem * sizeof))
                    return true
                }
            }
        }

        // Static array of raw serializable types
        else
        {
            var (raw, sizeof) = isPodFinal(typeElem)
            if raw
            {
                output.addBytes(@mkslice(cast(const ^u8) data, countElem * sizeof))
                return true
            }
        }

        return false
    }

    #[Warn("wrn0005", WarnLevel.Disable)]
    mtd endSequence()
    {
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd(NT) writeNative(value: NT)
    {
        #if NT == string
        {
            output.addNative(cast(u32) @countof(value))
            output.addBytes(value)
        }
        #else
        {
            output.addNative(value)
        }
    }
}