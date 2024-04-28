#global namespace Serialization.Write
#global public
using Swag

struct JSonOptions
{
    indentLevel:     u32 = 4
    saveBlanks:      bool
}

/*
A JSON writer.
*/
struct JSon
{
    options:           JSonOptions
    output:            *ConcatBuffer
    indent:            u32
    fmtInt:            StrConv.ConvertFormatInt
    fmtFloat:          StrConv.ConvertFormatFloat{fmt: `g`'u8}
    lastSeekValue:     ConcatBufferSeek
}

impl JSon
{
    mtd writeIndent()
    {
        if !options.saveBlanks do
            return
        loop indent * options.indentLevel do
            output.addNative(` `'u8)
    }

    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
    }

    #[Warn("Wrn0006", WarnLevel.Disable)]
    mtd beginRoot()
    {
    }

    #[Warn("Wrn0006", WarnLevel.Disable)]
    mtd endRoot()
    {
    }

    mtd beginSequence(_typeElem: typeinfo, _data: const *void, _countElem: u64)->bool
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

    mtd beginSection(_kind: SectionKind)
    {
        output.addBytes("{")
        lastSeekValue = output.getSeek()
        if options.saveBlanks
        {
            output.addBytes("\n")
            indent += 1
            writeIndent()
        }
    }

    mtd endSection()
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

    mtd beginStruct(_structType: typeinfo, _structData: const *u8)->bool
    {
        beginSection(.Ordered)
        return false
    }

    mtd endStruct()
    {
        endSection()
    }

    mtd beginElement(val: TypeValue)
    {
        output.addNative(`"`'u8)
        output.addBytes(val.name)
        output.addBytes("\":")
        if options.saveBlanks do
            output.addBytes(" ")
    }

    #[Warn("Wrn0006", WarnLevel.Disable)]
    mtd endElement() {}

    #[Warn("Wrn0006", WarnLevel.Disable)]
    mtd beginValue() {}

    mtd endValue()
    {
        lastSeekValue = output.getSeek()
        output.addBytes(",")
        if options.saveBlanks do
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
            StrConv.convertFloat(output, value, fmtFloat)
        }
        #elif Reflection.isInteger(NT)
        {
            StrConv.convertInt(output, value, fmtInt)
        }
        #elif NT == string
        {
            output.addNative(`"`'u8)
            output.addBytes(value)
            output.addNative(`"`'u8)
        }
        #else
        {
            #error "invalid type"
        }
    }
}
