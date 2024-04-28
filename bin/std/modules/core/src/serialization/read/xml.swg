#global namespace Serialization
#global namespace Read
using Swag, Core.Errors

/*
A simple XML reader.
In the case of a struct, the XML file must represent the structure layout (order of fields matters).
*/
public struct Xml
{
    stream:        ByteStream
    line, col:     u64 = 1
    propName:      String
}

public impl Xml
{
    mtd getError(val: string)->SyntaxError
    {
        var err: retval
        err.message = val
        err.line    = line
        err.col     = col
        err.seek    = stream.seek
        return err
    }

    mtd zapBlanks()
    {
        var c = stream.peekU8()
        while c and Latin1.isSpace(c)
        {
            stream.moveSeek()
            if c == `\n`
            {
                line += 1
                col = 1
            }
            else do
                col += 1
            c = stream.peekU8()
        }
    }

    mtd startRead(input: const [..] u8)
    {
        stream = ByteStream.openRead(input)
    }

    #[Warn("Wrn0006", WarnLevel.Disable)]
    mtd beginSequence(_typeElem: typeinfo, _data: *u8, _countElem: *u64)->bool throw
    {
        return false
    }

    #[Warn("Wrn0006", WarnLevel.Disable)]
    mtd endSequence() throw
    {
    }

    mtd toNextSequenceElement(_countElem: *u64)->bool
    {
        let c = stream.peekU8()
        return c != `<`
    }

    mtd beginRoot() throw
    {
        discard beginElement({})
    }

    mtd endRoot() throw
    {
        endElement()
    }

    mtd beginSection(_kind: SectionKind) throw
    {
        discard beginElement({})
    }

    mtd endSection() throw
    {
        endElement()
    }

    mtd beginStruct(_structType: typeinfo, _structData: const *u8)->bool throw
    {
        beginSection(.Ordered)
        return false
    }

    mtd endStruct() throw
    {
        endSection()
    }

    mtd beginElement(_val: TypeValue)->bool throw
    {
        zapBlanks()
        var c = stream.readU8()
        if c != `<` do
            throw getError("malformed element, missing opening [[<]]")

        propName.clear()
        c = stream.readU8()
        while c and c != `>`
        {
            propName += c
            c = stream.readU8()
        }

        if c != `>` do
            throw getError("malformed element, missing closing [[>]]")

        return true
    }

    mtd endElement() throw
    {
        zapBlanks()
        var c = stream.readU8()
        if c != `<` do
            throw getError("malformed element, missing opening [[<]]")

        while c and c != `>`
        {
            propName += c
            c = stream.readU8()
        }

        if c != `>` do
            throw getError("malformed element, missing closing [[>]]")
    }

    #[Warn("Wrn0006", WarnLevel.Disable)]
    {
        mtd beginValue() throw
        {
        }

        mtd endValue() throw
        {
        }

        mtd getVersion()->u32
        {
            return 0
        }
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd(NT) readNative()->NT throw
    {
        zapBlanks()
        #if NT == bool
        {
            var val = StrConv.parseBool(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == f32
        {
            var val = StrConv.parseF32(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == f64
        {
            var val = StrConv.parseF64(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s8
        {
            var val = StrConv.parseS8(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s16
        {
            var val = StrConv.parseS16(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s32
        {
            var val = StrConv.parseS32(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s64
        {
            var val = StrConv.parseS64(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u8
        {
            var val = StrConv.parseU8(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u16
        {
            var val = StrConv.parseU16(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u32
        {
            var val = StrConv.parseU32(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u64
        {
            var val = StrConv.parseU64(stream.remainReadToString())
            stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == Core.String
        {
            var result: retval
            var c       = stream.peekU8()
            while c and c != `<`
            {
                c = stream.readU8()
                result += c
                c = stream.peekU8()
            }

            return result
        }
        #else
        {
            #error "invalid type"
        }
    }
}
