#global namespace Serialization.Write
using Swag

public struct JSon
{
    using base: Serializer
    indent:     u32
    fmtInt:     StrConv.ConvertFormatInt
    fmtFloat:   StrConv.ConvertFormatFloat{fmt: "g"'u8}
}

public impl ISerializer for JSon
{
    private mtd writeIndent()
    {
        loop indent
            output.addByte(" "'u8)
    }

    mtd startElement(elem: const *Swag.TypeValue)
    {
        output.addByte("\""'u8)
        output.addBytes(elem.name)
        output.addBytes("\": ")
    }

    mtd startSequence(typeElem: typeinfo, countElem: uint, data: const *u8)->bool
    {
        output.addBytes("[\n")
        indent += 1
        writeIndent()
        return false
    }

    mtd endSequence()
    {
        indent -= 1
        output.addBytes("]")
    }

    mtd startStruct(type: typeinfo, data: const *u8)->bool
    {
        output.addBytes("{\n")
        indent += 1
        writeIndent()
        return false
    }

    mtd endStruct()
    {
        output.addBytes("}")
        indent -= 1
    }

    mtd endValue(type: typeinfo)
    {
        output.addBytes(",\n")
        writeIndent()
    }

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
}