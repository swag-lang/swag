#global namespace Serialization.Write
#global public
using Swag

enum TagBinSignatures
{
    SignatureFat = 0xCAFECAFE
}

struct TagBinSection
{
    startSeek:  ConcatBufferSeek
    fields:     Array'(const *TypeValue)
    mapSeek:    HashTable'(string, ConcatBufferSeek)
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
    }

    mtd endRoot()
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

    mtd beginStruct(type: typeinfo, data: const *u8)->bool
    {
        return false
    }

    mtd endStruct()
    {
    }

    mtd beginFields(fields: Array'(const *TypeValue), structType: typeinfo, structData: const *u8)->bool
    {
        sec := sections.emplaceAddress(1)
        @init(sec)

        output.addNative(TagBinSignatures.SignatureFat)
        output.addNative(fields.count)

        sec.fields = fields
        sec.startSeek = output.getSeek()

        // Fat
        visit field: fields
        {
            output.addNative(getCrc(field.name))
            // Will be replaced with the seek to the corresopnding startElement
            output.addNative(0'uint)
        }

        return false
    }

    mtd endFields()
    {
        curSeek := output.getSeek()
        defer output.setSeek(curSeek)

        // Patch fat table
        sec := sections.backPtr()
        output.setSeek(sec.startSeek)
        visit field: sec.fields
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
        if sections.isEmpty()
            return

        // Register the current seek for the fat generation
        sec := sections.backPtr()
        Debug.assert(!sec.mapSeek.contains(name))
        sec.mapSeek.add(name, output.getSeek())
    }

    mtd endElement()
    {
    }

    mtd beginValue(type: typeinfo)
    {
    }

    mtd endValue()
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