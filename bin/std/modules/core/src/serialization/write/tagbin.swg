#global namespace Serialization.Write
#global public
using Swag

enum TagBinSignatures
{
    File = 0xC0DEC0DE
    Fat  = 0xCAFECAFE
}

struct TagBinSection
{
    startSeek:  ConcatBufferSeek
    mapSeek:    HashTable'(string, ConcatBufferSeek)
    raw:        bool
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
        output.addNative(TagBinSignatures.File)
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
        sec := sections.emplaceAddress(1)
        @init(sec)
        sec.raw = true
    }

    mtd endSection()
    {
        sections.removeBack()
    }

    mtd beginFields(fields: Array'(const *TypeValue), structType: typeinfo, structData: const *u8)->bool
    {
        sec := sections.emplaceAddress(1)
        @init(sec)

        output.addNative(TagBinSignatures.Fat)
        output.addNative(fields.count)

        sec.startSeek = output.getSeek()

        // Fat
        visit field: fields
        {
            output.addNative(getCrc(field.name))
            output.addNative(0'uint) // Will be replaced with the seek to the corresponding startElement
        }

        return false
    }

    mtd endFields(fields: Array'(const *TypeValue))
    {
        curSeek := output.getSeek()
        defer output.setSeek(curSeek)

        // Patch fat table
        sec := sections.backPtr()
        output.setSeek(sec.startSeek)
        visit field: fields
        {
            // zap crc
            output.seek(4)

            p := sec.mapSeek.find(field.name)
            Debug.assert(p != null)

            // Write linear seek inside the buffer
            output.addNative(output.getOffset(p.value))
        }

        sections.removeBack()
    }

    mtd beginElement(name: string, type: typeinfo)
    {
        // Outside a section
        if !sections.isEmpty()
        {
            // Register the current seek for the fat generation
            sec := sections.backPtr()
            Debug.assert(!sec.mapSeek.contains(name))
            sec.mapSeek.add(name, output.getSeek())
        }

        // Write element signature
        output.addNative(getCrc(name))
        output.addNative(type.crc32)
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
        output.addNative(countElem)

        // Dynamic array of raw serializable types
        // Just write them in one batch
        if typeElem.kind == .Struct
        {
            structType := cast(const *TypeInfoStruct) typeElem
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType
                (raw, sizeof) := isRawSerializable(valueType)

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
            (raw, sizeof) := isRawSerializable(typeElem)
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