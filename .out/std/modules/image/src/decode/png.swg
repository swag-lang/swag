// https://github.com/odin-lang/Odin/blob/master/core/image/png/png.odin
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
	bitDepth:           u8
	colorType:          ColorType = ?
	compressionMethod:  u8
	filterMethod:       u8
	interlaceMethod:    InterlaceMethod = ?
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
    dstPixelsSize:  uint
    chunkTRNS:      Chunk
    seenTRSN:       bool
    seenBKGD:       bool
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
        if header.compressionMethod != 0
            throw "invalid compression"
        if header.filterMethod != 0
            throw "invalid filter"
        if header.interlaceMethod != .None and header.interlaceMethod != .Adam7
            throw "invalid interlace method"

        using header
        switch cast(u8) colorType
        {
        case 0:
            if bitDepth != 1 and bitDepth != 2 and bitDepth != 4 and bitDepth != 8 and bitDepth != 16
                throw "invalid color depth"

        case 2, 4, 6:
            if bitDepth != 8 and bitDepth != 16
                throw "invalid color depth"

        case 3:
            if bitDepth != 1 and bitDepth != 2 and bitDepth != 4 and bitDepth != 8
                throw "invalid color depth"

        default:
            throw "invalid color type"
        }

        if header.bitDepth > 8
            throw "unsupported 16 bpp"

        if header.colorType & .Paletted
        {
            throw "unsupported paletted image"
        }
        else if header.colorType & .Color
        {
            inChannels = 3
            outChannels = 3
        }
        else
        {
            inChannels = 1
            outChannels = 3
        }

        if header.colorType & .Alpha
        {
            inChannels += 1
            outChannels += 1
        }
    }

    func filterPaeth(left, up, upLeft: u8)->u8
    {
        aa := cast(s16) left
        bb := cast(s16) up
        cc := cast(s16) upLeft
        p  := aa + bb - cc
        pa := math.abs(p - aa)
        pb := math.abs(p - bb)
        pc := math.abs(p - cc)
        if pa <= pb and pa <= pc
            return left
        if pb <= pc
            return up
        return upLeft
    }

    func defilter8(using self) throw
    {
        rowStride := cast(uint) inChannels * header.width
        src := dcmpBuf.buffer
        dest := dstPixels + rowStride * (header.height - 1)

        var tmpRow: Array'u8
        tmpRow.resize(rowStride)
        memory.clear(tmpRow.buffer, rowStride)
        up := tmpRow.buffer

        loop header.height
        {
            nk := rowStride - inChannels
            filter := cast(RowFilter) src[0]
            src += 1

            switch filter
            {
            case None:
                memory.copy(dest, src, rowStride)

            case Sub:
                loop i: inChannels
                    dest[i] = src[i]
                loop k: nk
                    dest[inChannels + k] = bitcast(u8) (src[inChannels + k] + dest[k])

            case Up:
                loop k: rowStride
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
                loop i: inChannels
                {
                    paeth := filterPaeth(0, up[i], 0)
                    dest[i] = bitcast(u8) (src[i] + paeth)
                }
                loop k: nk
                {
                    paeth := filterPaeth(dest[k], up[inChannels + k], up[k])
                    dest[inChannels + k] = bitcast(u8) (src[inChannels + k] + paeth)
                }
                break
            }

            src += rowStride
            up = dest
            dest -= rowStride
        }
    }

    func defilter(using self) throw
    {
        if header.interlaceMethod != .Adam7
        {
            if header.bitDepth < 8
            {
                throw "unsupported bit depth"
                //throw defilter_less_than_8(buf)
                //img.depth = 8;
            }
            else if header.bitDepth == 8
                try defilter8()
            else
                throw "unsupported 16 bits depth" //defilter_16(buf)
        }
        else
        {
            throw "interlace not supported"
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
        seenIEND   := false
        seenIDAT   := false
        chIsHere   := false

        while !seenIEND
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
                dstPixelsSize = header.width * header.height * inChannels
                dstPixels = memory.alloc(dstPixelsSize)

            case TRNS:
                seenTRSN = true
                outChannels += 1 // transparency mask, add an alpha channel
                chunkTRNS = try readChunk(ch)

            case BKGD:
                seenBKGD = true
                chunk := try readChunk(ch)

            case IEND:
                seenIEND = true
                chunk := try readChunk(ch)

            case IDAT:
                if seenIDAT throw "invalid IDAT chunk"
                while ch.type == IDAT
                {
                    chunk := try readChunk(ch)
                    iDataBuf.add(chunk.datas)
                    stream.peek32Be(cast(*u8) &ch.length)
                    stream.peek32(cast(*u8) &ch.type)
                    if stream.eof throw "invalid chunk"
                }

                chIsHere = true
                seenIDAT = true
                break

            default:
                chunk := try readChunk(ch)
            }

            firstChunk = false
        }

        if !seenIDAT throw "missing IDAT chunk"

        // deflate content
        var dec: compress.ZLibDecoder
        dcmpBuf = try dec.decompress(iDataBuf.toSlice())

        // defilter content
        try defilter()

        // Initialize image depending on number of wanted channels
        if outChannels == 4
            result.init(cast(s32) header.width, cast(s32) header.height, PixelFormat.RGBA8)
        else
            result.init(cast(s32) header.width, cast(s32) header.height, PixelFormat.RGB8)
        result.allocator = @getcontext().allocator

        // If source == wanted channels, then we are done, dstPixels already contains everything
        if inChannels == outChannels
        {
            result.pixels = dstPixels
            return result
        }

        try convert(&result)
        return result
    }

    func convert(using self, result: *Image) throw
    {
        result.pixels = cast(*u8) memory.alloc(result.size)
        defer memory.free(dstPixels, dstPixelsSize)

        // Grayscale
        if inChannels == 1
        {
            result.visitPixels(&dstPixels)
            {
                src := :cast(**u8) userData
                src += x + y * image.width
                pix[0] = src[0]
                pix[1] = src[0]
                pix[2] = src[0]
                if image.bpp == 32 pix[3] = 255
            }
        }

        // Grayscale with alpha channel
        else if inChannels == 2
        {
            result.visitPixels(&dstPixels)
            {
                src := :cast(**u8) userData
                src += (x * 2) + (y * image.width * 2)
                pix[0] = src[0]
                pix[1] = src[0]
                pix[2] = src[0]
                if image.bpp == 32 pix[3] = src[1]
            }
        }

        // Color without alpha to alpha: use a transparency mask
        else if inChannels == 3
        {
            if !seenTRSN throw "missing TRSN chunk"
            key := cast(const *u8) @dataof(chunkTRNS.datas)
            key0 := key[1]
            key1 := key[3]
            key2 := key[5]
            pm := @{dstPixels, key0, key1, key2}
            result.visitPixels(&pm)
            {
                params := cast(*@typeof(pm)) userData
                src := params.dstPixels
                src += (x * 3) + (y * image.width * 3)
                pix[0] = src[0]
                pix[1] = src[1]
                pix[2] = src[2]
                if src[0] == params.key0 and src[1] == params.key1 and src[2] == params.key2
                    pix[3] = 0
                else
                    pix[3] = 255
            }
        }
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