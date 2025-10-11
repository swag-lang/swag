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
    stackTags:     Array'string
}

public impl Xml
{
    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd getVersion()->u32
    {
        return 0
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd isTextual()->bool
    {
        return true
    }

    mtd readTag()->string throw
    {
        .zapBlanks()

        var c = .stream.readU8()
        if c != '<' do
            throw .getError("malformed tag, missing opening [[<]]")

        let start = .stream.getSeek()
        c = .stream.readU8()
        while c and c != '>' do
            c = .stream.readU8()
        if c != '>' do
            throw .getError("malformed tag, missing closing [[>]]")

        let end = .stream.getSeek() - 1
        return Latin1.trim(cast(string) .stream.readBuffer[start until end])
    }

    mtd readStartTag()->string throw
    {
        let tag = .readTag()
        .stackTags.add(tag)
        return tag
    }

    mtd readEndTag() throw
    {
        let tag   = .readTag()
        let start = .stackTags.popBack()
        if tag[1 to ] != start do
            throw .getError(Format.toString("invalid end tag [[%]] for [[%]]", tag, start))
    }

    mtd getError(val: string)->SyntaxError
    {
        var err: retval
        err.message = Errors.mkString(val)
        err.line    = .line
        err.col     = .col
        err.seek    = .stream.seek
        return err
    }

    mtd zapBlanks()
    {
        var c = .stream.peekU8()
        while c and Latin1.isSpace(c)
        {
            .stream.moveSeek()
            if c == '\n'
            {
                .line += 1
                .col = 1
            }
            else
            {
                .col += 1
            }

            c = .stream.peekU8()
        }
    }

    mtd startRead(input: const [..] u8)
    {
        .stream = ByteStream.openRead(input)
    }

    mtd toNextSequenceElement(_countElem: *u64)->bool
    {
        let c = .stream.peekU8()
        return c != '<'
    }

    mtd beginSection(_kind: SectionKind) throw
    {
        discard .readTag()
    }

    mtd endSection() throw
    {
        discard .readTag()
    }

    mtd beginRoot() throw
    {
        discard .readStartTag()
    }

    mtd endRoot() throw
    {
        .readEndTag()
    }

    mtd beginElement(val: TypeValue)->bool throw
    {
        let tag = .readStartTag()
        if tag != val.name do
            throw .getError("invalid property name")
        return true
    }

    mtd endElement(_val: TypeValue) throw
    {
        .readEndTag()
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    {
        mtd beginStruct(structType: typeinfo, structData: const *u8)->bool throw { return false; }
        mtd endStruct() throw {}
        mtd beginSequence(_typeElem: typeinfo, _data: *u8, _countElem: *u64)->bool throw { return false; }
        mtd endSequence() throw {}
        mtd beginValue() throw {}
        mtd endValue() throw {}
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd(NT) readNative()->NT throw
    {
        .zapBlanks()
        #if NT == bool
        {
            let val = StrConv.parseBool(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == f32
        {
            let val = StrConv.parseF32(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == f64
        {
            let val = StrConv.parseF64(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s8
        {
            let val = StrConv.parseS8(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s16
        {
            let val = StrConv.parseS16(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s32
        {
            let val = StrConv.parseS32(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s64
        {
            let val = StrConv.parseS64(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u8
        {
            let val = StrConv.parseU8(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u16
        {
            let val = StrConv.parseU16(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u32
        {
            let val = StrConv.parseU32(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u64
        {
            let val = StrConv.parseU64(.stream.remainReadToString())
            .stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == Core.String
        {
            var result: retval
            var c       = .stream.peekU8()
            while c and c != '<'
            {
                c = .stream.readU8()
                result += c
                c = .stream.peekU8()
            }

            return result
        }
        #else
        {
            #error("invalid type")
        }
    }
}
