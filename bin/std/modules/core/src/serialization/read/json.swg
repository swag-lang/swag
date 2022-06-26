#global namespace Serialization.Read
using Swag

public struct JSon
{
    stream:     ByteStream
    line, col:  uint = 1
    propName:   String
}

public impl JSon
{
    mtd zapBlanks()
    {
        c := stream.peekU8()
        while c and Latin1.isSpace(c)
        {
            stream.seekNext()
            if c == "\n"'u8
            {
                line += 1
                col = 1
            }
            else
                col += 1
            c = stream.peekU8()
        }
    }

    mtd startRead(input: const [..] u8)
    {
        stream.init(input)
    }

    mtd startElement(elem: const *Swag.TypeValue) throw
    {
        zapBlanks()
        c := stream.readU8()
        if c != "\""'u8
            throw "malformed property name (missing opening '\"')"

        propName.clear()
        c = stream.peekU8()
        while c and c != "\""'u8
        {
            propName += c
            stream.seekNext()
            c = stream.peekU8()
        }

        if c != "\""'u8
            throw "malformed property name (missing closing '\"')"

        zapBlanks()
        c = stream.readU8()
        if c != ":"'u8
            throw "malformed property (missing ':' after name)"
    }

    mtd endElement(elem: const *Swag.TypeValue)
    {
    }

    mtd startSequence(typeElem: typeinfo, countElem: uint, data: const *u8)->bool
    {
        return false
    }

    mtd endSequence()
    {
    }

    mtd startStruct(type: typeinfo, data: const *u8)->bool throw
    {
        zapBlanks()
        c := stream.readU8()
        if c != "{"'u8
            throw "malformed struct (missing opening '{')"

        return false
    }

    mtd endStruct() throw
    {
        zapBlanks()
        c := stream.readU8()
        if c != "}"'u8
            throw "malformed struct (missing closing '}')"
    }

    mtd startValue(type: typeinfo)
    {
    }

    mtd endValue(type: typeinfo)
    {
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd readBool()->bool
    {
        return false
    }

    mtd readF32()->f32
    {
        return 0
    }

    mtd readF64()->f64
    {
        return 0
    }

    mtd readS8()->s8
    {
        return 0
    }

    mtd readS16()->s16
    {
        return 0
    }


    mtd readS32()->s32
    {
        return 0
    }

    mtd readS64()->s64
    {
        return 0
    }

    mtd readU8()->u8
    {
        return 0
    }

    mtd readU16()->u16
    {
        return 0
    }


    mtd readU32()->u32
    {
        return 0
    }

    mtd readU64()->u64
    {
        return 0
    }

    mtd readString()->string
    {
        return ""
    }
}