#global namespace Serialization.Write
#global public
using Swag

struct JSonOptions
{
    indentLevel: u32 = 4
    saveBlanks:  bool = true
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
            output.addByte(" "'u8)
    }

    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
    }

    mtd beginSequence(typeElem: typeinfo, countElem: uint, data: const *u8)->bool
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
        output.setSeek(lastSeekValue)
        if options.saveBlanks
        {
            output.addBytes("\n")
            indent -= 1
            writeIndent()
        }

        output.addBytes("]")
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
        output.setSeek(lastSeekValue)
        if options.saveBlanks
        {
            output.addBytes("\n")
            indent -= 1
            writeIndent()
        }

        output.addBytes("}")
    }

    mtd beginElement(name: string, type: typeinfo)
    {
        output.addByte("\""'u8)
        output.addBytes(name)
        output.addBytes("\":")
        if options.saveBlanks
            output.addBytes(" ")
    }

    mtd endElement()
    {
    }

    mtd beginValue(type: typeinfo)
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

    mtd writeBool(value: bool)
    {
        output.addBytes(value ? "true" : "false")
    }

    mtd writeF32(value: f32)
    {
        fmtFloat.value = value
        StrConv.convertFloat(output, fmtFloat)
    }

    mtd writeF64(value: f64)
    {
        fmtFloat.value = value
        StrConv.convertFloat(output, fmtFloat)
    }

    mtd writeU8(value: u8)
    {
        fmtInt.value = value
        StrConv.convertInt(output, fmtInt)
    }

    mtd writeU16(value: u16)
    {
        fmtInt.value = value
        StrConv.convertInt(output, fmtInt)
    }

    mtd writeU32(value: u32)
    {
        fmtInt.value = value
        StrConv.convertInt(output, fmtInt)
    }

    mtd writeU64(value: u64)
    {
        fmtInt.value = value
        StrConv.convertInt(output, fmtInt)
    }

    mtd writeS8(value: s8)
    {
        fmtInt.value = value
        StrConv.convertInt(output, fmtInt)
    }

    mtd writeS16(value: s16)
    {
        fmtInt.value = value
        StrConv.convertInt(output, fmtInt)
    }

    mtd writeS32(value: s32)
    {
        fmtInt.value = value
        StrConv.convertInt(output, fmtInt)
    }

    mtd writeS64(value: s64)
    {
        fmtInt.value = value
        StrConv.convertInt(output, fmtInt)
    }

    mtd writeString(value: string)
    {
        output.addByte("\""'u8)
        output.addBytes(value)
        output.addByte("\""'u8)
    }
}