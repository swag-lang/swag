#global namespace Serialization.Write
using Swag

public struct JSon
{
    using base: Serializer
    indent:     u32
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
}