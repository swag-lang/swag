#global marked
#global namespace Serialization
#global namespace Read
using Swag, Core.Errors

/*
A simple JSON reader.
In the case of a struct, the JSON file must represent the structure layout (order of fields matters).
*/
public struct JSon
{
    stream:        ByteStream
    line, col:     u64 = 1
    propName:      String
}

public impl JSon
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

    mtd getError(val: string)->SyntaxError
    {
        var err: retval
        err.message = val
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
            else:
                me.col += 1
            c = me.stream.peekU8()
        }
    }

    mtd startRead(input: const [..] u8)
    {
        me.stream = ByteStream.openRead(input)
    }

    mtd beginSequence(_typeElem: typeinfo, _data: *u8, _countElem: *u64)->bool throw
    {
        me.zapBlanks()
        let c = me.stream.readU8()
        if c != '[':
            throw me.getError("malformed array, missing opening [[[]]")
        return false
    }

    mtd endSequence() throw
    {
        me.zapBlanks()
        let c = me.stream.readU8()
        if c != ']':
            throw me.getError("malformed array; missing closing [[]]]")
    }

    mtd toNextSequenceElement(_countElem: *u64)->bool
    {
        me.zapBlanks()
        let c = me.stream.peekU8()
        return c != ']'
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd beginRoot() throw
    {
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endRoot() throw
    {
    }

    mtd beginSection(_kind: SectionKind) throw
    {
        me.zapBlanks()
        let c = me.stream.readU8()
        if c != '{':
            throw me.getError("malformed struct, missing opening [[{]]")
    }

    mtd endSection() throw
    {
        me.zapBlanks()
        let c = me.stream.readU8()
        if c != '}':
            throw me.getError("malformed struct, missing closing [[}]]")
    }

    mtd beginStruct(_structType: typeinfo, _structData: const *u8)->bool throw
    {
        me.beginSection(.Ordered)
        return false
    }

    mtd endStruct() throw
    {
        me.endSection()
    }

    mtd beginElement(val: TypedValue)->bool throw
    {
        me.zapBlanks()
        var c = me.stream.readU8()
        if c != '"':
            throw me.getError("malformed property name, missing opening [[\"]]")

        me.propName.clear()
        c = me.stream.readU8()
        while c and c != '"'
        {
            me.propName += c
            c = me.stream.readU8()
        }

        if me.propName != val.name:
            throw me.getError("invalid property name")

        if c != '"':
            throw me.getError("malformed property name, missing closing [[\"]]")

        me.zapBlanks()
        c = me.stream.readU8()
        if c != ':':
            throw me.getError("malformed property, missing [[:]] after name")

        return true
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd endElement(_val: TypedValue) throw
    {
    }

    #[Warning("Wrn0006", WarningLevel.Disable)]
    mtd beginValue() throw
    {
    }

    mtd endValue() throw
    {
        me.zapBlanks()
        let c = me.stream.peekU8()
        if c == ',':
            me.stream.moveSeek()
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

            me.zapBlanks()
            var c = me.stream.readU8()
            if c != '"'
            {
                if c == 'n' and
                   me.stream.readU8() == 'u' and
                   me.stream.readU8() == 'l' and
                   me.stream.readU8() == 'l'
                {
                    return result
                }

                throw getError("malformed string, missing opening [[\"]]")
            }

            c = me.stream.peekU8()
            while c and c != '"'
            {
                c = me.stream.readU8()
                result += c
                c = me.stream.peekU8()
            }

            c = me.stream.readU8()
            if c != '"':
                throw getError("malformed string, missing closing [[\"]]")

            return result
        }
        #else
        {
            #error("invalid type")
        }
    }
}
