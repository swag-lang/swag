#global namespace Serialization.Write
#global public
using Swag

struct JSonOptions
{
    indentLevel: u32 = 4
    saveBlanks:  bool
}

struct JSon
{
    options:        JSonOptions
    output:         ByteStream
    indent:         u32
    fmtInt:         StrConv.ConvertFormatInt
    fmtFloat:       StrConv.ConvertFormatFloat{fmt: "g"'u8}
    lastSeekValue:  uint
    convBuffer:     ConcatBuffer
}

impl JSon
{
    mtd writeIndent()
    {
        if !options.saveBlanks
            return
        loop indent * options.indentLevel
            output.writeNative(" "'u8)
    }

    mtd startWrite(buf: *Array'u8)
    {
        output = ByteStream.openWrite(buf)
    }

    mtd beginRoot()
    {
        discard beginStruct(null, null)
    }

    mtd endRoot()
    {
        output.setSeek(lastSeekValue) // Remove last ','
        endStruct()
    }

    mtd beginSequence(typeElem: typeinfo, data: const *void, countElem: uint)->bool
    {
        output.writeBytes("[")
        lastSeekValue = output.getSeek()
        if options.saveBlanks
        {
            output.writeBytes("\n")
            indent += 1
            writeIndent()
        }

        return false
    }

    mtd endSequence()
    {
        output.setSeek(lastSeekValue)
        if options.saveBlanks
        {
            output.writeBytes("\n")
            indent -= 1
            writeIndent()
        }

        output.writeBytes("]")
        lastSeekValue = output.getSeek()
    }

    mtd beginStruct(type: typeinfo, data: const *u8)->bool
    {
        output.writeBytes("{")
        lastSeekValue = output.getSeek()
        if options.saveBlanks
        {
            output.writeBytes("\n")
            indent += 1
            writeIndent()
        }

        return false
    }

    mtd endStruct()
    {
        output.setSeek(lastSeekValue)
        if options.saveBlanks
        {
            output.writeBytes("\n")
            indent -= 1
            writeIndent()
        }

        output.writeBytes("}")
        lastSeekValue = output.getSeek()
    }

    mtd beginSection()
    {
    }

    mtd endSection()
    {
    }

    mtd beginFields(fields: Array'(const *TypeValue), structType: typeinfo, structData: const *u8)->bool
    {
        return false
    }

    mtd endFields(fields: Array'(const *TypeValue))
    {
    }

    mtd beginElement(name: string, type: typeinfo)
    {
        output.writeNative("\""'u8)
        output.writeBytes(name)
        output.writeBytes("\":")
        if options.saveBlanks
            output.writeBytes(" ")
    }

    mtd endElement()
    {
    }

    mtd beginValue()
    {
    }

    mtd endValue()
    {
        lastSeekValue = output.getSeek()
        output.writeBytes(",")
        if options.saveBlanks
            output.writeBytes("\n")
        writeIndent()
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd(NT) writeNative(value: NT)
    {
        #if NT == bool
        {
            output.writeBytes(value ? "true" : "false")
        }
        #elif Reflection.isFloat(NT)
        {
            fmtFloat.value = value
            convBuffer.clear() // Horrible
            StrConv.convertFloat(&convBuffer, fmtFloat)
            output.writeBytes(convBuffer.toSlice())
        }
        #elif Reflection.isInteger(NT)
        {
            fmtInt.value = value
            convBuffer.clear() // Horrible
            StrConv.convertInt(&convBuffer, fmtInt)
            output.writeBytes(convBuffer.toSlice())
        }
        #elif NT == string
        {
            output.writeNative("\""'u8)
            output.writeBytes(value)
            output.writeNative("\""'u8)
        }
        #else
        {
            #assert(false, "invalid type")
        }
    }
}