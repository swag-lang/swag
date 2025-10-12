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
    mtd isWriting() => .isWrite
    #[Swag.Inline]
    mtd isReading() => !.isWrite

    // Start encoding
    mtd startWrite(encoder: IEncoder, output: *ConcatBuffer) throw
    {
        .encode  = encoder
        .isWrite = true
        .encode.start(output)
    }

    // Start decoding
    mtd startRead(decoder: IDecoder, input: const [..] u8) throw
    {
        .decode  = decoder
        .isWrite = false
        .decode.start(input)
    }

    // End encoding/decoding
    mtd end() throw
    {
        if .isWrite do
            .encode.end()
        else do
            .decode.end()
    }

    // Start a new section
    mtd beginSection(kind = SectionKind.Unordered) throw
    {
        if .isWrite do
            .encode.beginSection(kind)
        else do
            .decode.beginSection(kind)
    }

    // End the previous section
    mtd endSection() throw
    {
        if .isWrite do
            .encode.endSection()
        else do
            .decode.endSection()
    }

    // Serialize one value
    #[Swag.Optimize(.All, false), Swag.Overload]
    mtd(MT) serialize(name: string, crc: u32, res: *MT) throw
    {
        var value: Swag.TypeValue
        value.name        = name
        value.crc         = crc
        value.pointedType = #typeof(MT)

        if .isWrite
        {
            .encode.beginField(value)
            defer #noerr .encode.endField(value)

            #if MT == s8 do
                .encode.writeS8(dref res)
            #elif MT == s16 do
                .encode.writeS16(dref res)
            #elif MT == s32 do
                .encode.writeS32(dref res)
            #elif MT == s64 do
                .encode.writeS64(dref res)
            #elif MT == u8 do
                .encode.writeU8(dref res)
            #elif MT == u16 do
                .encode.writeU16(dref res)
            #elif MT == u32 or MT == rune do
                .encode.writeU32(dref res)
            #elif MT == u64 do
                .encode.writeU64(dref res)
            #elif MT == bool do
                .encode.writeBool(dref res)
            #elif MT == f32 do
                .encode.writeF32(dref res)
            #elif MT == f64 do
                .encode.writeF64(dref res)
            #else do
                .encode.write(res, #typeof(MT))
        }
        else
        {
            if !.decode.beginField(value) do
                return
            defer #noerr .decode.endField(value)

            #if MT == s8 do
                dref res = .decode.readS8()
            #elif MT == s16 do
                dref res = .decode.readS16()
            #elif MT == s32 do
                dref res = .decode.readS32()
            #elif MT == s64 do
                dref res = .decode.readS64()
            #elif MT == u8 do
                dref res = .decode.readU8()
            #elif MT == u16 do
                dref res = .decode.readU16()
            #elif MT == u32 or MT == rune do
                dref res = .decode.readU32()
            #elif MT == u64 do
                dref res = .decode.readU64()
            #elif MT == bool do
                dref res = .decode.readBool()
            #elif MT == f32 do
                dref res = .decode.readF32()
            #elif MT == f64 do
                dref res = .decode.readF64()
            #else do
                .decode.read(res, #typeof(MT))
        }
    }

    // Serialize one value
    #[Swag.Mixin, Swag.Overload]
    mtd(MT) serialize(name: string, res: *MT) throw
    {
        .serialize(name, Hash.Crc32.compute(name), res)
    }
}
