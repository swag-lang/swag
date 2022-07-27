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
        stream = ByteStream.openRead(input)
    }

    mtd beginSequence(typeElem: typeinfo, data: *u8, countElem: *uint)->bool throw
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

    mtd toNextSequenceElement(countElem: *uint)->bool
    {
        zapBlanks()
        c := stream.peekU8()
        return c != "]"'u8
    }

    mtd beginRoot() throw
    {
        zapBlanks()
        c := stream.readU8()
        if c != "{"'u8
            throw "malformed root (missing opening '{')"
    }

    mtd endRoot() throw
    {
        zapBlanks()
        c := stream.readU8()
        if c != "}"'u8
            throw "malformed root (missing closing '}')"
    }

    mtd beginStruct(data: *u8, type: typeinfo)->bool throw
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

    mtd beginSection()
    {
    }

    mtd endSection()
    {
    }

    mtd beginFields(fields: Array'(const *TypeValue), structType: typeinfo, structData: const *u8)->bool throw
    {
        return false
    }

    mtd endFields(fields: Array'(const *TypeValue)) throw
    {
    }

    mtd beginElement(val: TypeValue)->bool throw
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

        if propName != val.name
            throw "invalid property name"

        if c != "\""'u8
            throw "malformed property name (missing closing '\"')"

        zapBlanks()
        c = stream.readU8()
        if c != ":"'u8
            throw "malformed property (missing ':' after name)"

        return true
    }

    mtd endElement() throw
    {
    }

    mtd beginValue() throw
    {
    }

    mtd endValue() throw
    {
        zapBlanks()
        c := stream.peekU8()
        if c == ","'u8
            stream.seekNext()
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd(NT) readNative()->NT throw
    {
        zapBlanks()
        #if NT == bool
        {
            val := StrConv.parseBool(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == f32
        {
            val := StrConv.parseF32(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == f64
        {
            val := StrConv.parseF64(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == s8
        {
            val := StrConv.parseS8(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == s16
        {
            val := StrConv.parseS16(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == s32
        {
            val := StrConv.parseS32(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == s64
        {
            val := StrConv.parseS64(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == u8
        {
            val := StrConv.parseU8(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == u16
        {
            val := StrConv.parseU16(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == u32
        {
            val := StrConv.parseU32(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == u64
        {
            val := StrConv.parseU64(stream.remainReadToString())
            stream.seekNext(acast val.eat)
            return val.value
        }
        #elif NT == Core.String
        {
            var result: retval

            zapBlanks()
            c := stream.readU8()
            if c != "\""'u8
                throw "malformed string (missing opening '\"')"

            c = stream.peekU8()
            while c and c != "\""'u8
            {
                c = stream.readU8()
                result += c
                c = stream.peekU8()
            }

            c = stream.readU8()
            if c != "\""'u8
                throw "malformed string (missing closing '\"')"

            return result
        }
        #else
        {
            #assert(false, "invalid type")
        }
    }
}