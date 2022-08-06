#global namespace Serialization.Write
#global public
using Swag

enum TagBinSignatures
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
    raw:        bool
    final:      bool
}

struct TagBin
{
    output:   *ConcatBuffer
    sections: Array'TagBinSection
}

impl TagBin
{
    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
        sections.clear()
    }

    mtd beginRoot()
    {
        output.addNative(TagBinSignatures.Root)
    }

    mtd endRoot()
    {
        Debug.assert(sections.isEmpty())
    }

    mtd beginStruct(type: typeinfo, data: const *u8)->bool
    {
        return false
    }

    mtd endStruct()
    {
    }

    mtd beginSection()
    {
        sec := sections.emplaceInitAddress()
        sec.raw = true
    }

    mtd endSection()
    {
        sections.removeBack()
    }

    mtd beginFields(fields: Array'(const *TypeValue), structType: typeinfo, structData: const *u8)->bool
    {
        sec := sections.emplaceInitAddress()

        if  Reflection.hasAttribute(structType, Serialization.Final) or
            Reflection.hasAttribute(structType, Serialization.PodFinal)
        {
            sec.final = true
            return false
        }

        // Struct version
        output.addNative(TagBinSignatures.Version)
        version := Reflection.getAttributeValue(structType, Serialization.Version, "v")
        if version == null
            output.addNative(0'u32)
        else
            output.addNative(cast(u32) version)

        // Seek to the fat
        output.addNative(TagBinSignatures.Fat)
        sec.startSeek = output.getSeek()
        output.addNative(0'uint)

        return false
    }

    mtd endFields(fields: Array'(const *TypeValue))
    {
        sec := sections.backPtr()
        if sec.final
        {
            sections.removeBack()
            return
        }

        // Patch fat table seek
        curSeek := output.getSeek()
        output.setSeek(sec.startSeek)
        output.addNative(output.getOffset(curSeek))
        output.setSeek(curSeek)

        // Write fat table
        output.addNative(TagBinSignatures.Fat)
        output.addNative(sec.mapSeek.count)

        visit k, v: sec.mapSeek
        {
            // Crc
            output.addNative(k)

            // Linear seek inside the buffer
            output.addNative(output.getOffset(v))
        }

        sections.removeBack()
    }

    mtd beginElement(val: TypeValue)
    {
        // Outside a section
        if !sections.isEmpty()
        {
            sec := sections.backPtr()

            // If section is final, then we just have values without versionning
            if sec.final
                return

            // If section is raw, then we do not have a fat, this are not fields
            if !sec.raw
            {
                // Register the current seek for the fat generation
                Debug.assert(!sec.mapSeek.contains(val.crc32))
                sec.mapSeek.add(val.crc32, output.getSeek())
            }
        }

        // Write element signature
        output.addNative(val.crc32)
        output.addNative(val.pointedType.crc32)
    }

    mtd endElement()
    {
    }

    mtd beginValue()
    {
    }

    mtd endValue()
    {
    }

    mtd beginSequence(typeElem: typeinfo, data: const *void, countElem: uint)->bool
    {
        output.addNative(TagBinSignatures.Sequence)
        output.addNative(countElem)

        // Dynamic array of raw serializable types
        // Just write them in one batch
        if typeElem.kind == .Struct
        {
            structType := cast(const *TypeInfoStruct) typeElem
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType
                (raw, sizeof) := isPodFinal(valueType)

                if raw
                {
                    bufArr := cast(const *Array'u8) data
                    output.addBytes(@mkslice(bufArr.buffer, countElem * sizeof))
                    return true
                }
            }
        }

        // Static array of raw serializable types
        else
        {
            (raw, sizeof) := isPodFinal(typeElem)
            if raw
            {
                output.addBytes(@mkslice(cast(const *u8) data, countElem * sizeof))
                return true
            }
        }

        return false
    }

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