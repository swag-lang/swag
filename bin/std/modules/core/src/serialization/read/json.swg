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
        c = stream.readU8()
        while c and c != "\""'u8
        {
            propName += c
            c = stream.readU8()
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

    mtd startSequence(typeElem: typeinfo, countElem: uint, data: const *u8)->bool throw
    {
        zapBlanks()
        c := stream.readU8()
        if c != "["'u8
            throw "malformed array (missing opening '[')"

        return false
    }

    mtd endSequence() throw
    {
        zapBlanks()
        c := stream.readU8()
        if c != "]"'u8
            throw "malformed array (missing closing ']')"
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
        zapBlanks()
        c := stream.peekU8()
        if c == ","'u8
            stream.seekNext()
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd readBool()->bool throw
    {
        zapBlanks()
        val := StrConv.parseBool(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }

    mtd readF32()->f32
    {
        zapBlanks()
        val := catch StrConv.parseF32(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }

    mtd readF64()->f64
    {
        return 0
    }

    mtd readS8()->s8
    {
        zapBlanks()
        val := catch StrConv.parseS8(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }

    mtd readS16()->s16
    {
        zapBlanks()
        val := catch StrConv.parseS16(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }


    mtd readS32()->s32
    {
        zapBlanks()
        val := catch StrConv.parseS32(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }

    mtd readS64()->s64
    {
        zapBlanks()
        val := catch StrConv.parseS64(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }

    mtd readU8()->u8
    {
        zapBlanks()
        val := catch StrConv.parseU8(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }

    mtd readU16()->u16
    {
        zapBlanks()
        val := catch StrConv.parseU16(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }


    mtd readU32()->u32
    {
        zapBlanks()
        val := catch StrConv.parseU32(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }

    mtd readU64()->u64
    {
        zapBlanks()
        val := catch StrConv.parseU64(stream.toString())
        stream.seekNext(acast val.eat)
        return val.value
    }
}