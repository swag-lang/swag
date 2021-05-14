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

const Adam7OrigX:     []int = @[0,4,0,2,0,1,0]
const Adam7OrigY:     []int = @[0,0,4,0,2,0,1]
const Adam7SpacingX:  []int = @[8,8,4,4,2,2,1]
const Adam7SpacingY:  []int = @[8,8,8,4,4,2,2]

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
    decOptions:     const *DecodeOptions
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
    orderDefilter:  bool
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
            throw "invalid compression method"
        if header.filterMethod != 0
            throw "invalid filter method"
        if header.interlaceMethod != .None and header.interlaceMethod != .Adam7
            throw "invalid interlace method"

        using header
        if colorType & .Paletted throw "unsupported paletted image"
        if colorType > 7 throw "invalid color type"
        if bitDepth > 8 throw "unsupported bit depth"
        if bitDepth != 1 and bitDepth != 2 and bitDepth != 4 and bitDepth != 8 throw "invalid bit depth"

        outChannels = 3
        if header.colorType & .Color
            inChannels = 3
        else
            inChannels = 1
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

    // Expand 1, 2, 4 bits to 8 bits
    func expand8(using self, width, height: u32, output: *u8)
    {
        rowStrideIn  := getStrideIn(width);
        rowStrideOut := inChannels * width

        orgDest := output
        srcOffset := rowStrideOut - rowStrideIn

        // 1 bit => 8 bits
        if header.bitDepth == 1
        {
            loop height
            {
                dest := orgDest
                orgDest += rowStrideOut
                src := dest + srcOffset
                k := rowStrideOut
                while k >= 8
                {
                    c := src[0]
                    dest[0] = bitcast(u8) ((c >>,safe 7) * 255)
                    dest[1] = bitcast(u8) (((c >>,safe 6) & 1) * 255)
                    dest[2] = bitcast(u8) (((c >>,safe 5) & 1) * 255)
                    dest[3] = bitcast(u8) (((c >>,safe 4) & 1) * 255)
                    dest[4] = bitcast(u8) (((c >>,safe 3) & 1) * 255)
                    dest[5] = bitcast(u8) (((c >>,safe 2) & 1) * 255)
                    dest[6] = bitcast(u8) (((c >>,safe 1) & 1) * 255)
                    dest[7] = bitcast(u8) ((c & 1) * 255)
                    dest += 8
                    src += 1
                    k -= 8
                }
                if k > 0
                {
                    c := src[0]
                    dest[0] = bitcast(u8) ((c >>,safe 7) * 255)
                    if k > 1 dest[1] = bitcast(u8) (((c >>,safe 6) & 1) * 255)
                    if k > 2 dest[2] = bitcast(u8) (((c >>,safe 5) & 1) * 255)
                    if k > 3 dest[3] = bitcast(u8) (((c >>,safe 4) & 1) * 255)
                    if k > 4 dest[4] = bitcast(u8) (((c >>,safe 3) & 1) * 255)
                    if k > 5 dest[5] = bitcast(u8) (((c >>,safe 2) & 1) * 255)
                    if k > 6 dest[6] = bitcast(u8) (((c >>,safe 1) & 1) * 255)
                }
            }
        }

        // 2 bits => 8 bits
        else if header.bitDepth == 2
        {
            loop height
            {
                dest := orgDest
                orgDest += rowStrideOut
                src := dest + srcOffset
                k := rowStrideOut
                while k >= 2
                {
                    c := src[0]
                    dest[0] = bitcast(u8) ((c >>,safe 6) * 85)
                    dest[1] = bitcast(u8) (((c >>,safe 4) & 3) * 85)
                    dest[2] = bitcast(u8) (((c >>,safe 2) & 3) * 85)
                    dest[3] = bitcast(u8) ((c & 3) * 85)
                    dest += 4
                    src += 1
                    k -= 4
                }
                if k > 0
                {
                    c := src[0]
                    dest[0] = bitcast(u8) ((c >>,safe 6) * 85)
                    if k > 1 dest[1] = bitcast(u8) (((c >>,safe 4) & 3) * 85)
                    if k > 2 dest[2] = bitcast(u8) (((c >>,safe 2) & 3) * 85)
                }
            }
        }

        // 4 bits => 8 bits
        else if header.bitDepth == 4
        {
            loop height
            {
                dest := orgDest
                orgDest += rowStrideOut
                src := dest + srcOffset
                k := rowStrideOut
                while k >= 2
                {
                    c := src[0]
                    dest[0] = bitcast(u8) ((c >>,safe 4) * 17)
                    dest[1] = bitcast(u8) ((c & 15) * 17)
                    dest += 2
                    src += 1
                    k -= 2
                }
                if k > 0
                {
                    dest[0] = bitcast(u8) ((src[0] >>,safe 4) * 17)
                }
            }
        }
    }

    func getStrideIn(using self, width: u32)->uint
    {
        if header.bitDepth < 8
            return cast(uint) ((inChannels * width * header.bitDepth) + 7) / 8
        return cast(uint) inChannels * width
    }

    func defilter8(using self, width, height: u32, input, output: *u8)
    {
        rowStrideIn := getStrideIn(width)
        rowStrideOut := cast(uint) inChannels * width
        destOffset := rowStrideOut - rowStrideIn

        src := input
        dest := output
        if !orderDefilter dest += rowStrideOut * (height - 1)

        // A temporary line "before" the first one, if filter needs one
        var tmpRow: Array'u8
        tmpRow.resize(rowStrideOut)
        memory.clear(tmpRow.buffer, rowStrideOut)
        up := tmpRow.buffer

        nk := rowStrideIn - inChannels
        loop height
        {
            filter := cast(RowFilter) src[0]
            src += 1

            destOff := dest + destOffset
            switch filter
            {
            case None:
                memory.copy(destOff, src, rowStrideIn)

            case Sub:
                loop i: inChannels
                    destOff[i] = src[i]
                loop k: nk
                    destOff[inChannels + k] = bitcast(u8) (src[inChannels + k] + destOff[k])

            case Up:
                loop k: rowStrideIn
                    destOff[k] = bitcast(u8) (src[k] + up[k])

            case Average:
                loop i: inChannels
                {
                    avg := up[i] >>,safe 1
                    destOff[i] = bitcast(u8) (src[i] + avg)
                }
                loop k: nk
                {
                    avg := cast(u8) ((cast(u16) (up[inChannels + k]) + cast(u16) (destOff[k])) >>,safe 1)
                    destOff[inChannels + k] = bitcast(u8) (src[inChannels + k] + avg)
                }

            case Paeth:
                loop i: inChannels
                {
                    paeth := filterPaeth(0, up[i], 0)
                    destOff[i] = bitcast(u8) (src[i] + paeth)
                }
                loop k: nk
                {
                    paeth := filterPaeth(destOff[k], up[inChannels + k], up[k])
                    destOff[inChannels + k] = bitcast(u8) (src[inChannels + k] + paeth)
                }
            }

            src += rowStrideIn
            up = destOff
            if orderDefilter
                dest += rowStrideOut
            else
                dest -= rowStrideOut
        }
    }

    func defilter(using self, width, height: u32, input, output: *u8)
    {
        if header.bitDepth < 8
        {
            defilter8(width, height, input, output)
            expand8(width, height, output)
            return
        }

        if header.bitDepth == 8
        {
            defilter8(width, height, input, output)
            return
        }
    }

    func adam7(using self)
    {
        var tmpArr: Array'u8
        input := dcmpBuf.buffer
        loop p: 7
        {
			x := (header.width  - Adam7OrigX[p] + Adam7SpacingX[p] - 1) / Adam7SpacingX[p]
			y := (header.height - Adam7OrigY[p] + Adam7SpacingY[p] - 1) / Adam7SpacingY[p]
            if x <= 0 or y <= 0 continue

            tmpArr.resize(x * y * inChannels)
            memory.clear(tmpArr.buffer, tmpArr.count)

            defilter(cast(u32) x, cast(u32) y, input, tmpArr.buffer)

            t := tmpArr.buffer
            loop j: y
            {
                loop i: x
                {
                    out_y   := header.height - ((j * Adam7SpacingY[p]) + Adam7OrigY[p]) - 1
                    out_x   := (i * Adam7SpacingX[p]) + Adam7OrigX[p]
                    out_off := out_y * header.width * inChannels
                    out_off += out_x * inChannels
                    loop z: inChannels
                        dstPixels[out_off + z] = t[z]
                    t += inChannels
                }
            }

            stride := (getStrideIn(cast(u32) x) + 1) * y
            input += stride
        }
    }

    func defilter(using self)
    {
        if header.interlaceMethod != .Adam7
        {
            orderDefilter = false
            defilter(header.width, header.height, dcmpBuf.buffer, dstPixels)
        }
        else
        {
            orderDefilter = true
            adam7()
        }
    }

    func doIt(using self, buffer: const [..] u8)->Image throw
    {
        stream.init(buffer)

        var signature: u64 = ?
        stream.peekBytes(@sizeof(signature), cast(*u8) &signature)
        if signature != 0x0a1a0a0d474e5089 throw "not a png"

        var result:     retval
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
                if decOptions.decodePixels
                {
                    dstPixelsSize = header.width * header.height * inChannels
                    dstPixels = memory.alloc(dstPixelsSize)
                }

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

            default:
                chunk := try readChunk(ch)
            }

            firstChunk = false
        }

        if !seenIDAT throw "missing IDAT chunk"

        // Initialize image depending on number of wanted channels
        if outChannels == 4
            result.init(cast(s32) header.width, cast(s32) header.height, PixelFormat.RGBA8)
        else
            result.init(cast(s32) header.width, cast(s32) header.height, PixelFormat.RGB8)

        // We are done if user does not want pixels
        if !decOptions.decodePixels
            return result

        // deflate content
        var dec: compress.ZLibDecoder
        dcmpBuf = try dec.decompress(iDataBuf.toSlice())

        // defilter content
        defilter()

        // If source == wanted channels, then we are done, dstPixels already contains everything
        result.allocator = @getcontext().allocator
        if inChannels == outChannels
        {
            result.pixels = dstPixels
            return result
        }

        convert(&result)
        return result
    }

    func convert(using self, result: *Image)
    {
        result.pixels = cast(*u8) memory.alloc(result.size)
        defer memory.free(dstPixels, dstPixelsSize)

        // Grayscale without alpha channel, no transparency
        if inChannels == 1 and !seenTRSN
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

        // Grayscale without alpha channel, but transparency
        else if inChannels == 1 and seenTRSN
        {
            key := chunkTRNS.datas[1]
            scale := header.bitDepth == 1 ? 255'u8 : header.bitDepth == 2 ? 85'u8 : 17'u8
            key *= scale
            pm := @{dstPixels, key}
            result.visitPixels(&pm)
            {
                params := cast(*@typeof(pm)) userData
                src := params.dstPixels
                src += x + y * image.width
                pix[0] = src[0]
                pix[1] = src[0]
                pix[2] = src[0]
                pix[3] = src[0] == params.key ? 0 : 255
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

        // Color without alpha but transparency
        else if inChannels == 3 and seenTRSN
        {
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

    func decode(using self, buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        decOptions = options
        return try doIt(buffer)
    }
}