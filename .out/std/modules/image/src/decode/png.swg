#global namespace png
using swag, core

enum ChunkType: u32
{
    IHDR = @byteswap("I"'u32 << 24 | "H"'u32 << 16 | "D"'u32 << 8 | "R"'u32)
	PLTE = @byteswap("P"'u32 << 24 | "L"'u32 << 16 | "T"'u32 << 8 | "E"'u32)
	BKGD = @byteswap("b"'u32 << 24 | "K"'u32 << 16 | "G"'u32 << 8 | "D"'u32)
	TRNS = @byteswap("t"'u32 << 24 | "R"'u32 << 16 | "N"'u32 << 8 | "S"'u32)
	IDAT = @byteswap("I"'u32 << 24 | "D"'u32 << 16 | "A"'u32 << 8 | "T"'u32)

	ITXt = @byteswap("i"'u32 << 24 | "T"'u32 << 16 | "X"'u32 << 8 | "t"'u32)
	TEXt = @byteswap("t"'u32 << 24 | "E"'u32 << 16 | "X"'u32 << 8 | "t"'u32)
	ZTXt = @byteswap("z"'u32 << 24 | "T"'u32 << 16 | "X"'u32 << 8 | "t"'u32)

	ICCP = @byteswap("i"'u32 << 24 | "C"'u32 << 16 | "C"'u32 << 8 | "P"'u32)
	PHYs = @byteswap("p"'u32 << 24 | "H"'u32 << 16 | "Y"'u32 << 8 | "s"'u32)
	GAMA = @byteswap("g"'u32 << 24 | "A"'u32 << 16 | "M"'u32 << 8 | "A"'u32)
	TIME = @byteswap("t"'u32 << 24 | "I"'u32 << 16 | "M"'u32 << 8 | "E"'u32)

	SPLT = @byteswap("s"'u32 << 24 | "P"'u32 << 16 | "L"'u32 << 8 | "T"'u32)
	SRGB = @byteswap("s"'u32 << 24 | "R"'u32 << 16 | "G"'u32 << 8 | "B"'u32)
	HIST = @byteswap("h"'u32 << 24 | "I"'u32 << 16 | "S"'u32 << 8 | "T"'u32)
	CHRM = @byteswap("c"'u32 << 24 | "H"'u32 << 16 | "R"'u32 << 8 | "M"'u32)
	SBIT = @byteswap("s"'u32 << 24 | "B"'u32 << 16 | "I"'u32 << 8 | "T"'u32)

	EXIf = @byteswap("e"'u32 << 24 | "X"'u32 << 16 | "I"'u32 << 8 | "f"'u32)
    IEND = @byteswap("I"'u32 << 24 | "E"'u32 << 16 | "N"'u32 << 8 | "D"'u32)
	IDOT = @byteswap("i"'u32 << 24 | "D"'u32 << 16 | "O"'u32 << 8 | "T"'u32)
	CbGI = @byteswap("C"'u32 << 24 | "b"'u32 << 16 | "H"'u32 << 8 | "I"'u32)
}

enum RowFilter: u8
{
   None    = 0
   Sub     = 1
   Up      = 2
   Average = 3
   Paeth   = 4
}

struct ChunkHeader
{
    length: u32
    type:   ChunkType = ?
}

enum InterlaceMethod : u8
{
	None  = 0
	Adam7 = 1
}

#[enumflags]
enum ColorType: u8
{
    Paletted = 1
    Color    = 2
    Alpha    = 4
}

struct Header
{
	width:              u32
	height:             u32
	bit_depth:          u8
	color_type:         ColorType = ?
	compression_method: u8
	filter_method:      u8
	interlace_method:   InterlaceMethod = ?
}

#[opaque]
public struct Decoder
{
    stream:         compress.CompressStream
    header:         Header
    inChannels:     s32
    outChannels:    s32
    dcmpBuf:        Array'u8
    dstPixels:      *u8
}

struct Chunk
{
    header: ChunkHeader
    datas:  const [..] u8
    crc:    u32
}

impl Decoder
{
    func readChunk(using self, ch: ChunkHeader)->Chunk throw
    {
        var result: retval

        result.header = ch
        result.datas = @mkslice(stream.curPtr(), ch.length)
        stream.curByte += ch.length

        var crc: hash.Crc32
        crc.update(@mkslice(cast(*u8) &ch.type, 4))
        crc.update(result.datas)

        var vcrc: u32
        stream.peek32Be(cast(*u8) &vcrc)
        if vcrc != crc.crc throw "invalid checksum"

        return result
    }

    func readHeader(using self, ch: ChunkHeader) throw
    {
        chunk := try readChunk(ch)
        header = :cast(const *Header) @dataof(chunk.datas)
        header.width = math.bigEndianToNative(header.width)
        header.height = math.bigEndianToNative(header.height)

        if header.width == 0 or header.height == 0
            throw "invalid dimensions"
        if header.compression_method != 0
            throw "invalid compression"
        if header.filter_method != 0
            throw "invalid filter"
        if header.interlace_method != .None and header.interlace_method != .Adam7
            throw "invalid interlace method"

        using header
        switch cast(u8) color_type
        {
        case 0:
            if bit_depth != 1 and bit_depth != 2 and bit_depth != 4 and bit_depth != 8 and bit_depth != 16
                throw "invalid color depth"

        case 2, 4, 6:
            if bit_depth != 8 and bit_depth != 16
                throw "invalid color depth"

        case 3:
            if bit_depth != 1 and bit_depth != 2 and bit_depth != 4 and bit_depth != 8
                throw "invalid color depth"

        default:
            throw "invalid color type"
        }
    }

    func defilter_8(using self) throw
    {
        row_stride := cast(uint) inChannels * header.width
        src := dcmpBuf.buffer
        dest := dstPixels + row_stride * (header.height - 1)

        var tmpRow: Array'u8
        tmpRow.resize(row_stride)
        memory.clear(tmpRow.buffer, row_stride)
        up := tmpRow.buffer

        loop header.height
        {
            nk := row_stride - inChannels
            filter := cast(RowFilter) src[0]
            src += 1

            switch filter
            {
            case None:
                memory.copy(dest, src, row_stride)

            case Sub:
                loop i: inChannels
                    dest[i] = src[i]
                loop k: nk
                    dest[inChannels + k] = bitcast(u8) (src[inChannels + k] + dest[k])

            case Up:
                loop k: row_stride
                    dest[k] = bitcast(u8) (src[k] + up[k])
                break

            case Average:
                loop i: inChannels
                {
                    avg := up[i] >>,safe 1
                    dest[i] = bitcast(u8) (src[i] + avg)
                }
                loop k: nk
                {
                    avg := cast(u8) ((cast(u16) (up[inChannels + k]) + cast(u16) (dest[k])) >>,safe 1)
                    dest[inChannels + k] = bitcast(u8) (src[inChannels + k] + avg)
                }

            case Paeth:
                break
            }

            src += row_stride
            up = dest
            dest -= row_stride
        }
    }

    func defilter(using self) throw
    {
        if header.interlace_method != .Adam7
        {
            if header.bit_depth < 8
            {
                throw "unsupported"
                //throw defilter_less_than_8(buf)
                //img.depth = 8;
            }
            else if header.bit_depth == 8
                try defilter_8()
            else
                throw "unsupported" //defilter_16(buf)
        }
    }

    func doIt(using self, buffer: const [..] u8)->Image throw
    {
        stream.init(buffer)

        var signature: u64 = ?
        stream.peekBytes(@sizeof(signature), cast(*u8) &signature)
        if signature != 0x0a1a0a0d474e5089 throw "not a png"

        var result: retval

        var iDataBuf:   Array'u8
        var ch:         ChunkHeader = ?

        firstChunk := true
        seenIEnd   := false
        seenIDat   := false
        chIsHere   := false

        while !seenIEnd
        {
            // Peek a chunk header only if it's not already done
            if !chIsHere
            {
                stream.peek32Be(cast(*u8) &ch.length)
                stream.peek32(cast(*u8) &ch.type)
            }

            chIsHere = false
            if stream.eof throw "invalid chunk"

            switch ch.type
            {
            case IHDR:
                if !firstChunk throw "invalid first chunk"
                try readHeader(ch)

                if header.bit_depth > 8 throw "unsupported 16 bpp"
                if header.color_type & .Paletted throw "unsupported paletted image"

                if header.color_type & .Color
                {
                    inChannels = 3
                    outChannels = 3
                }

                if header.color_type & .Alpha
                {
                    inChannels += 1
                    outChannels += 1
                }

                if outChannels == 4
                    result = image.create(cast(s32) header.width, cast(s32) header.height, PixelFormat.RGBA8)
                else
                    result = image.create(cast(s32) header.width, cast(s32) header.height, PixelFormat.RGB8)
                dstPixels = result.pixels

            case IEND:
                chunk := try readChunk(ch)
                seenIEnd = true

            case IDAT:
                if seenIDat throw "invalid IDAT chunk"
                while ch.type == IDAT
                {
                    chunk := try readChunk(ch)
                    iDataBuf.add(chunk.datas)
                    stream.peek32Be(cast(*u8) &ch.length)
                    stream.peek32(cast(*u8) &ch.type)
                    if stream.eof throw "invalid chunk"
                }

                chIsHere = true
                seenIDat = true
                break

            default:
                chunk := try readChunk(ch)
            }

            firstChunk = false
        }

        if !seenIDat throw "missing IDAT chunk"

        // deflate content
        var dec: compress.ZLibDecoder
        dcmpBuf = try dec.decompress(iDataBuf.toSlice())

        try defilter()
        return result
    }
}

public impl IDecoder for Decoder
{
    func canDecode(using self, fileName: string)->bool
    {
        ext := path.getExtensionLowerCase(fileName)
        return ext == ".png"
    }

    func decode(using self, buffer: const [..] u8)->Image throw
    {
        return try doIt(buffer)
    }
}