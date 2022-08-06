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
    output:         *ConcatBuffer
    indent:         u32
    fmtInt:         StrConv.ConvertFormatInt
    fmtFloat:       StrConv.ConvertFormatFloat{fmt: "g"'u8}
    lastSeekValue:  ConcatBufferSeek
}

impl JSon
{
    mtd writeIndent()
    {
        if !options.saveBlanks
            return
        loop indent * options.indentLevel
            output.addNative(" "'u8)
    }

    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
    }

    mtd beginRoot()
    {
        discard beginStruct(null, null)
    }

    mtd endRoot()
    {
        output.setEndSeek(lastSeekValue) // Remove last ','
        endStruct()
    }

    mtd beginSequence(typeElem: typeinfo, data: const *void, countElem: uint)->bool
    {
        output.addBytes("[")
        lastSeekValue = output.getSeek()
        if options.saveBlanks
        {
            output.addBytes("\n")
            indent += 1
            writeIndent()
        }

        return false
    }

    mtd endSequence()
    {
        output.setEndSeek(lastSeekValue)
        if options.saveBlanks
        {
            output.addBytes("\n")
            indent -= 1
            writeIndent()
        }

        output.addBytes("]")
        lastSeekValue = output.getSeek()
    }

    mtd beginStruct(type: typeinfo, data: const *u8)->bool
    {
        output.addBytes("{")
        lastSeekValue = output.getSeek()
        if options.saveBlanks
        {
            output.addBytes("\n")
            indent += 1
            writeIndent()
        }

        return false
    }

    mtd endStruct()
    {
        output.setEndSeek(lastSeekValue)
        if options.saveBlanks
        {
            output.addBytes("\n")
            indent -= 1
            writeIndent()
        }

        output.addBytes("}")
        lastSeekValue = output.getSeek()
    }

    mtd beginSection()
    {
    }

    mtd endSection()
    {
    }

    mtd beginFields(structType: typeinfo, structData: const *u8)->bool
    {
        return false
    }

    mtd endFields()
    {
    }

    mtd beginElement(val: TypeValue)
    {
        output.addNative("\""'u8)
        output.addBytes(val.name)
        output.addBytes("\":")
        if options.saveBlanks
            output.addBytes(" ")
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
        output.addBytes(",")
        if options.saveBlanks
            output.addBytes("\n")
        writeIndent()
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd(NT) writeNative(value: NT)
    {
        #if NT == bool
        {
            output.addBytes(value ? "true" : "false")
        }
        #elif Reflection.isFloat(NT)
        {
            fmtFloat.value = value
            StrConv.convertFloat(output, fmtFloat)
        }
        #elif Reflection.isInteger(NT)
        {
            fmtInt.value = value
            StrConv.convertInt(output, fmtInt)
        }
        #elif NT == string
        {
            output.addNative("\""'u8)
            output.addBytes(value)
            output.addNative("\""'u8)
        }
        #else
        {
            #assert(false, "invalid type")
        }
    }
}