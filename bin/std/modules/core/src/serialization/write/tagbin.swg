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
    final:      bool
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
            ptr := freeSections.popBack()
            ptr.mapSeek.clear()
            ptr.final = false
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

    mtd beginStruct(type: typeinfo, data: const *u8)->bool
    {
        return false
    }

    mtd endStruct()
    {
    }

    mtd beginSection()
    {
        sec := newSection()

        // Will store the fat offset here
        output.addNative(TagBinSignatures.Fat)
        sec.startSeek = output.getSeek()
        output.addNative(0'uint)
    }

    mtd endSection()
    {
        sec := sections.back()
        if sec.final
        {
            releaseSection()
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

        releaseSection()
    }

    mtd beginFields(structType: typeinfo, structData: const *u8)->bool
    {
        if  Reflection.hasAttribute(structType, Serialization.Final) or
            Reflection.hasAttribute(structType, Serialization.PodFinal)
        {
            sec := newSection()
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

        beginSection()
        return false
    }

    mtd endFields()
    {
        endSection()
    }

    mtd beginElement(val: TypeValue)
    {
        // Outside a section
        if !sections.isEmpty()
        {
            sec := sections.back()

            // If section is final, then we just have values without versionning
            if sec.final
                return

            // Register the current seek for the fat generation
            Debug.assert(!sec.mapSeek.contains(val.crc32))
            sec.mapSeek.add(val.crc32, output.getSeek())
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