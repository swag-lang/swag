#global namespace Serialization
#global namespace Write
#global public
using Swag

enum TagBinSignatures: u32
{
    Root     = 0xC0DE0000
    Fat      = 0xC0DE0001
    Sequence = 0xC0DE0002
    Version  = 0xC0DE0003
    Raw      = 0xC0DE0004
    UnRaw    = 0xC0DE0005
}

struct TagBinSection
{
    startSeek:     ConcatBufferSeek
    mapSeek:       HashTable'(u32, ConcatBufferSeek)
    kind:          SectionKind
}

struct TagBin
{
    output:           *ConcatBuffer
    freeSections:     ArrayPtr'TagBinSection
    sections:         ArrayPtr'TagBinSection
}

internal impl TagBin
{
    mtd newSection()->*TagBinSection
    {
        if freeSections.count
        {
            let ptr = freeSections.popBack()
            ptr.mapSeek.clear()
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
}

impl TagBin
{
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd isTextual()->bool
    {
        return true
    }

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
        let sec = newSection()
        sec.kind = kind

        if sec.kind == .Unordered
        {
            output.addNative(TagBinSignatures.Fat)
            sec.startSeek = output.getSeek()
            output.addNative(0'u64) // Will store the fat offset here
            output.addNative(0'u64) // Will store the fat count here
        }
    }

    mtd endSection()
    {
        let sec = sections.back()

        if sec.kind == .Unordered
        {
            // Patch fat table seek and count
            var curSeek = output.getSeek()
            output.setSeek(sec.startSeek)
            output.addNative(ConcatBuffer.getOffset(curSeek))
            output.addNative(sec.mapSeek.count)
            output.setSeek(curSeek)

            // Write fat table
            output.addNative(TagBinSignatures.Fat)
            foreach k, v in sec.mapSeek
            {
                output.addNative(k) // Crc
                output.addNative(ConcatBuffer.getOffset(v)) // Linear seek inside the buffer
            }
        }

        releaseSection()
    }

    mtd beginStruct(structType: typeinfo, _structData: const *u8)->bool
    {
        let (pod, ?) = Serialization.isPodFinal(structType)
        if pod
        {
            output.addNative(TagBinSignatures.Raw)
            beginSection(.Raw)
        }
        else
        {
            // Struct version
            output.addNative(TagBinSignatures.Version)
            let version = Reflection.getAttributeValue(structType, Serialization.Version, "v")
            if version == null:
                output.addNative(0'u32)
            else:
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

        let sec = sections.back()

        // If section is raw, then we just have values without versionning
        if sec.kind == .Raw:
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

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endElement() {}

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd beginValue() {}

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endValue() {}

    mtd beginSequence(typeElem: typeinfo, data: const *void, countElem: u64)->bool
    {
        output.addNative(TagBinSignatures.Sequence)
        output.addNative(countElem)

        // Dynamic array of raw serializable types
        // Just write them in one batch
        if Reflection.isStructArray(typeElem)
        {
            let valueType = Reflection.getStructArrayType(typeElem)
            let (raw, sizeof) = isPodFinal(valueType)

            if raw
            {
                output.addNative(TagBinSignatures.Raw)
                let bufArr = cast(const *Array'u8) data
                output.addBytes(@mkslice(bufArr.buffer, countElem * sizeof))
                return true
            }
            else
            {
                output.addNative(TagBinSignatures.UnRaw)
            }
        }

        // Static array of raw serializable types
        else
        {
            let (raw, sizeof) = isPodFinal(typeElem)
            if raw
            {
                output.addNative(TagBinSignatures.Raw)
                output.addBytes(@mkslice(cast(const ^u8) data, countElem * sizeof))
                return true
            }
            else
            {
                output.addNative(TagBinSignatures.UnRaw)
            }
        }

        return false
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endSequence() {}

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
