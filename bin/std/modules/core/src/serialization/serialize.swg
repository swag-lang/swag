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

interface ISerialize
{
    mtd read(decoder: IDecoder)->bool throw;
    mtd write(encoder: IEncoder)->bool throw;
    mtd readElement(type: TypeValue, data: *void, decoder: IDecoder)->bool throw;
    mtd writeElement(type: TypeValue, data: const *void, encoder: IEncoder)->bool throw;
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
    mtd isWriting() => isWrite
    #[Swag.Inline]
    mtd isReading() => !isWrite

    // Start encoding
    mtd startWrite(encoder: IEncoder, output: *ConcatBuffer) throw
    {
        encode  = encoder
        isWrite = true
        encode.start(output)
    }

    // Start decoding
    mtd startRead(decoder: IDecoder, input: const [..] u8) throw
    {
        decode  = decoder
        isWrite = false
        decode.start(input)
    }

    // End encoding/decoding
    mtd end() throw
    {
        if isWrite:
            encode.end()
        else:
            decode.end()
    }

    // Start a new section
    mtd beginSection(kind = SectionKind.Unordered) throw
    {
        if isWrite:
            encode.beginSection(kind)
        else:
            decode.beginSection(kind)
    }

    // End the previous section
    mtd endSection() throw
    {
        if isWrite:
            encode.endSection()
        else:
            decode.endSection()
    }

    // Serialize one value
    #[Swag.Optimize("", false), Swag.Overload]
    mtd(MT) serialize(name: string, crc: u32, res: nn *MT) throw
    {
        var value: Swag.TypeValue
        value.name        = name
        value.crc         = crc
        value.pointedType = #typeof(MT)

        if isWrite
        {
            encode.beginField(value)
            defer<noerr> encode.endField()

            #if MT == s8:
                encode.writeS8(dref res)
            #elif MT == s16:
                encode.writeS16(dref res)
            #elif MT == s32:
                encode.writeS32(dref res)
            #elif MT == s64:
                encode.writeS64(dref res)
            #elif MT == u8:
                encode.writeU8(dref res)
            #elif MT == u16:
                encode.writeU16(dref res)
            #elif MT == u32 or MT == rune:
                encode.writeU32(dref res)
            #elif MT == u64:
                encode.writeU64(dref res)
            #elif MT == bool:
                encode.writeBool(dref res)
            #elif MT == f32:
                encode.writeF32(dref res)
            #elif MT == f64:
                encode.writeF64(dref res)
            #else:
                encode.write(res, #typeof(MT))
        }
        else
        {
            if !decode.beginField(value):
                return
            defer<noerr> decode.endField()

            #if MT == s8:
                dref res = decode.readS8()
            #elif MT == s16:
                dref res = decode.readS16()
            #elif MT == s32:
                dref res = decode.readS32()
            #elif MT == s64:
                dref res = decode.readS64()
            #elif MT == u8:
                dref res = decode.readU8()
            #elif MT == u16:
                dref res = decode.readU16()
            #elif MT == u32 or MT == rune:
                dref res = decode.readU32()
            #elif MT == u64:
                dref res = decode.readU64()
            #elif MT == bool:
                dref res = decode.readBool()
            #elif MT == f32:
                dref res = decode.readF32()
            #elif MT == f64:
                dref res = decode.readF64()
            #else:
                decode.read(res, #typeof(MT))
        }
    }

    // Serialize one value
    #[Swag.Mixin, Swag.Overload]
    mtd(MT) serialize(name: string, res: nn *MT) throw
    {
        serialize(name, Hash.Crc32.compute(name), res)
    }
}
