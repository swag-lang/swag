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
        me.zapBlanks()

        var c = me.stream.readU8()
        if c != '<':
            throw me.getError("malformed tag, missing opening [[<]]")

        let start = me.stream.getSeek()
        c = me.stream.readU8()
        while c and c != '>':
            c = me.stream.readU8()
        if c != '>':
            throw me.getError("malformed tag, missing closing [[>]]")

        let end = me.stream.getSeek() - 1
        return Latin1.trim(cast(string) me.stream.readBuffer[start..<end])
    }

    mtd readStartTag()->string throw
    {
        let tag = me.readTag()
        me.stackTags.add(tag)
        return tag
    }

    mtd readEndTag() throw
    {
        let tag   = me.readTag()
        let start = me.stackTags.popBack()
        if tag[1..] != start:
            throw me.getError(Format.toString("invalid end tag [[%]] for [[%]]", tag, start))
    }

    mtd getError(val: string)->SyntaxError
    {
        var err: retval
        err.message = Errors.mkString(val)
        err.line    = me.line
        err.col     = me.col
        err.seek    = me.stream.seek
        return err
    }

    mtd zapBlanks()
    {
        var c = me.stream.peekU8()
        while c and Latin1.isSpace(c)
        {
            me.stream.moveSeek()
            if c == '\n'
            {
                me.line += 1
                me.col = 1
            }
            else
            {
                me.col += 1
            }

            c = me.stream.peekU8()
        }
    }

    mtd startRead(input: const [..] u8)
    {
        me.stream = ByteStream.openRead(input)
    }

    mtd toNextSequenceElement(_countElem: *u64)->bool
    {
        let c = me.stream.peekU8()
        return c != '<'
    }

    mtd beginSection(_kind: SectionKind) throw
    {
        discard me.readTag()
    }

    mtd endSection() throw
    {
        discard me.readTag()
    }

    mtd beginRoot() throw
    {
        discard me.readStartTag()
    }

    mtd endRoot() throw
    {
        me.readEndTag()
    }

    mtd beginElement(val: TypeValue)->bool throw
    {
        let tag = me.readStartTag()
        if tag != val.name:
            throw me.getError("invalid property name")
        return true
    }

    mtd endElement(_val: TypeValue) throw
    {
        me.readEndTag()
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
        me.zapBlanks()
        #if NT == bool
        {
            var val = StrConv.parseBool(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == f32
        {
            var val = StrConv.parseF32(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == f64
        {
            var val = StrConv.parseF64(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s8
        {
            var val = StrConv.parseS8(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s16
        {
            var val = StrConv.parseS16(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s32
        {
            var val = StrConv.parseS32(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == s64
        {
            var val = StrConv.parseS64(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u8
        {
            var val = StrConv.parseU8(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u16
        {
            var val = StrConv.parseU16(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u32
        {
            var val = StrConv.parseU32(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == u64
        {
            var val = StrConv.parseU64(me.stream.remainReadToString())
            me.stream.moveSeek(val.eat)
            return val.value
        }
        #elif NT == Core.String
        {
            var result: retval
            var c       = me.stream.peekU8()
            while c and c != '<'
            {
                c = me.stream.readU8()
                result += c
                c = me.stream.peekU8()
            }

            return result
        }
        #else
        {
            #error("invalid type")
        }
    }
}
