#global export
#global namespace Serialization
using Swag

// Set the struct version number
#[AttrUsage(AttributeUsage.Struct)]
attr Version(v: u32)

// The struct does not need versionning
#[AttrUsage(AttributeUsage.Struct)]
attr Final()

// Accept this other name for the struct field or the struct itself
#[AttrUsage(AttributeUsage.Struct | AttributeUsage.StructVariable | AttributeUsage.Multi)]
attr Alias(name: string)

// Do not serialize a struct or a field
#[AttrUsage(AttributeUsage.Struct | AttributeUsage.StructVariable)]
attr NoSerialize()

// Compress a struct or a field
#[AttrUsage(AttributeUsage.Struct | AttributeUsage.StructVariable)]
attr Compress()

interface ISerialize
{
    mtd read(decoder: IDecoder)->bool throw;
    mtd write(encoder: IEncoder)->bool throw;
    mtd readElement(type: TypedValue, data: *void, decoder: IDecoder)->bool throw;
    mtd writeElement(type: TypedValue, data: const *void, encoder: IEncoder)->bool throw;
    mtd postRead(data: *void, decoder: IDecoder) throw;
}

struct Serializer
{
    encode:      IEncoder
    decode:      IDecoder
    isWrite:     bool
}

impl Serializer
{
    #[Swag.Inline]
    mtd isWriting() => me.isWrite
    #[Swag.Inline]
    mtd isReading() => !me.isWrite

    // Start encoding
    mtd startWrite(encoder: IEncoder, output: *ConcatBuffer) throw
    {
        me.encode  = encoder
        me.isWrite = true
        me.encode.start(output)
    }

    // Start decoding
    mtd startRead(decoder: IDecoder, input: const [..] u8) throw
    {
        me.decode  = decoder
        me.isWrite = false
        me.decode.start(input)
    }

    // End encoding/decoding
    mtd end() throw
    {
        if me.isWrite:
            me.encode.end()
        else:
            me.decode.end()
    }

    // Start a new section
    mtd beginSection(kind = SectionKind.Unordered) throw
    {
        if me.isWrite:
            me.encode.beginSection(kind)
        else:
            me.decode.beginSection(kind)
    }

    // End the previous section
    mtd endSection() throw
    {
        if me.isWrite:
            me.encode.endSection()
        else:
            me.decode.endSection()
    }

    // Serialize one value
    #[Swag.Optimize("", false), Swag.Overload]
    mtd(MT) serialize(name: string, crc: u32, res: *MT) throw
    {
        var value: Swag.TypedValue
        value.name        = name
        value.crc         = crc
        value.pointedType = #typeof(MT)

        if me.isWrite
        {
            me.encode.beginField(value)
            defer #noerr me.encode.endField(value)

            #if MT == s8:
                me.encode.writeS8(dref res)
            #elif MT == s16:
                me.encode.writeS16(dref res)
            #elif MT == s32:
                me.encode.writeS32(dref res)
            #elif MT == s64:
                me.encode.writeS64(dref res)
            #elif MT == u8:
                me.encode.writeU8(dref res)
            #elif MT == u16:
                me.encode.writeU16(dref res)
            #elif MT == u32 or MT == rune:
                me.encode.writeU32(dref res)
            #elif MT == u64:
                me.encode.writeU64(dref res)
            #elif MT == bool:
                me.encode.writeBool(dref res)
            #elif MT == f32:
                me.encode.writeF32(dref res)
            #elif MT == f64:
                me.encode.writeF64(dref res)
            #else:
                me.encode.write(res, #typeof(MT))
        }
        else
        {
            if !me.decode.beginField(value):
                return
            defer #noerr me.decode.endField(value)

            #if MT == s8:
                dref res = me.decode.readS8()
            #elif MT == s16:
                dref res = me.decode.readS16()
            #elif MT == s32:
                dref res = me.decode.readS32()
            #elif MT == s64:
                dref res = me.decode.readS64()
            #elif MT == u8:
                dref res = me.decode.readU8()
            #elif MT == u16:
                dref res = me.decode.readU16()
            #elif MT == u32 or MT == rune:
                dref res = me.decode.readU32()
            #elif MT == u64:
                dref res = me.decode.readU64()
            #elif MT == bool:
                dref res = me.decode.readBool()
            #elif MT == f32:
                dref res = me.decode.readF32()
            #elif MT == f64:
                dref res = me.decode.readF64()
            #else:
                me.decode.read(res, #typeof(MT))
        }
    }

    // Serialize one value
    #[Swag.Mixin, Swag.Overload]
    mtd(MT) serialize(name: string, res: *MT) throw
    {
        me.serialize(name, Hash.Crc32.compute(name), res)
    }
}
