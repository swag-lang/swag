/*
    :LICENCE
    This code is based on the Odin library (https://github.com/odin-lang/Odin/blob/master/core/image/png/png.odin)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Png
using Swag, Core, Core.Errors

private
{
    enum ChunkType: u32
    {
        IHDR = @byteswap('I''u32 << 24 | 'H''u32 << 16 | 'D''u32 << 8 | 'R''u32)
        PLTE = @byteswap('P''u32 << 24 | 'L''u32 << 16 | 'T''u32 << 8 | 'E''u32)
        BKGD = @byteswap('b''u32 << 24 | 'K''u32 << 16 | 'G''u32 << 8 | 'D''u32)
        TRNS = @byteswap('t''u32 << 24 | 'R''u32 << 16 | 'N''u32 << 8 | 'S''u32)
        IDAT = @byteswap('I''u32 << 24 | 'D''u32 << 16 | 'A''u32 << 8 | 'T''u32)

        ITXt = @byteswap('i''u32 << 24 | 'T''u32 << 16 | 'X''u32 << 8 | 't''u32)
        TEXt = @byteswap('t''u32 << 24 | 'E''u32 << 16 | 'X''u32 << 8 | 't''u32)
        ZTXt = @byteswap('z''u32 << 24 | 'T''u32 << 16 | 'X''u32 << 8 | 't''u32)

        ICCP = @byteswap('i''u32 << 24 | 'C''u32 << 16 | 'C''u32 << 8 | 'P''u32)
        PHYs = @byteswap('p''u32 << 24 | 'H''u32 << 16 | 'Y''u32 << 8 | 's''u32)
        GAMA = @byteswap('g''u32 << 24 | 'A''u32 << 16 | 'M''u32 << 8 | 'A''u32)
        TIME = @byteswap('t''u32 << 24 | 'I''u32 << 16 | 'M''u32 << 8 | 'E''u32)

        SPLT = @byteswap('s''u32 << 24 | 'P''u32 << 16 | 'L''u32 << 8 | 'T''u32)
        SRGB = @byteswap('s''u32 << 24 | 'R''u32 << 16 | 'G''u32 << 8 | 'B''u32)
        HIST = @byteswap('h''u32 << 24 | 'I''u32 << 16 | 'S''u32 << 8 | 'T''u32)
        CHRM = @byteswap('c''u32 << 24 | 'H''u32 << 16 | 'R''u32 << 8 | 'M''u32)
        SBIT = @byteswap('s''u32 << 24 | 'B''u32 << 16 | 'I''u32 << 8 | 'T''u32)

        EXIf = @byteswap('e''u32 << 24 | 'X''u32 << 16 | 'I''u32 << 8 | 'f''u32)
        IEND = @byteswap('I''u32 << 24 | 'E''u32 << 16 | 'N''u32 << 8 | 'D''u32)
        IDOT = @byteswap('i''u32 << 24 | 'D''u32 << 16 | 'O''u32 << 8 | 'T''u32)
        CbGI = @byteswap('C''u32 << 24 | 'b''u32 << 16 | 'H''u32 << 8 | 'I''u32)
    }

    const Adam7OrigX:    [?] s64 = [0, 4, 0, 2, 0, 1, 0]
    const Adam7OrigY:    [?] s64 = [0, 0, 4, 0, 2, 0, 1]
    const Adam7SpacingX: [?] s64 = [8, 8, 4, 4, 2, 2, 1]
    const Adam7SpacingY: [?] s64 = [8, 8, 8, 4, 4, 2, 2]

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
        length:     u32
        type:       ChunkType = undefined
    }

    enum InterlaceMethod: u8
    {
        None  = 0
        Adam7 = 1
    }

    #[Swag.EnumFlags]
    enum ColorType: u8
    {
        Grayscale = 0
        Paletted  = 1
        Color     = 2
        Alpha     = 4
    }

    struct Header
    {
        width:                 u32
        height:                u32
        bitDepth:              u8
        colorType:             ColorType
        compressionMethod:     u8
        filterMethod:          u8
        interlaceMethod:       InterlaceMethod = undefined
    }

    struct Chunk
    {
        header:     ChunkHeader
        datas:      const [..] u8
        crc:        u32
    }
}

#[Opaque]
public struct Decoder
{
    decOptions:        const *DecodeOptions
    stream:            Compress.BitStream
    header:            Header
    inChannels:        s32
    outChannels:       s32
    dcmpBuf:           Array'u8
    dstPixels:         [*] u8
    dstPixelsSize:     u64
    chunkTRNS:         Chunk
    palette:           [256] u32
    paletteCount:      s32
    seenTRSN:          bool
    seenBKGD:          bool
    seenPLTE:          bool
    orderDefilter:     bool
}

impl Decoder
{
    mtd readChunk(ch: ChunkHeader)->Chunk throw
    {
        var result: retval

        result.header = ch
        if .stream.curPtr() + ch.length > .stream.startPtr() + .stream.size() do
            throw InvalidFormatError{"invalid chunk"}
        result.datas = @mkslice(.stream.curPtr(), ch.length)
        .stream.curByte += ch.length

        var crc: Hash.Crc32
        crc.update(@mkslice(cast(const [*] u8) &ch.type, 4))
        crc.update(result.datas)

        var vcrc: u32
        .stream.peek32Be(cast([*] u8) &vcrc)
        if vcrc != crc.crc do
            throw InvalidFormatError{"invalid checksum"}

        return result
    }

    mtd readHeader(ch: ChunkHeader) throw
    {
        let chunk = .readChunk(ch)
        .header        = dref cast(const *Header) @dataof(chunk.datas)
        .header.width  = Math.bigEndianToNative(.header.width)
        .header.height = Math.bigEndianToNative(.header.height)

        with me.header
        {
            if .width == 0 or .height == 0 do
                throw InvalidFormatError{"invalid dimensions"}
            if .compressionMethod != 0 do
                throw InvalidFormatError{"invalid compression mtd"}
            if .filterMethod != 0 do
                throw InvalidFormatError{"invalid filter mtd"}
            if .interlaceMethod != .None and .interlaceMethod != .Adam7 do
                throw InvalidFormatError{"invalid interlace mtd"}

            if .colorType > 7 do
                throw InvalidFormatError{"invalid color type"}
            if .bitDepth > 8 do
                throw UnsupportedFormatError{"unsupported bit depth"}

            me.outChannels = 3
            if me.header.colorType & .Paletted do
                me.inChannels = 1
            elif me.header.colorType & .Color do
                me.inChannels = 3
            else do
                me.inChannels = 1

            if me.header.colorType & .Alpha
            {
                me.inChannels += 1
                me.outChannels += 1
            }

            if .bitDepth != 1 and .bitDepth != 2 and .bitDepth != 4 and .bitDepth != 8 do
                throw UnsupportedFormatError{"invalid bit depth"}
        }
    }

    func filterPaeth(left, up, upLeft: u8)->u8
    {
        let aa = cast(s16) left
        let bb = cast(s16) up
        let cc = cast(s16) upLeft
        let p  = aa + bb - cc
        let pa = Math.abs(cast(s16) (p - aa))
        let pb = Math.abs(cast(s16) (p - bb))
        let pc = Math.abs(cast(s16) (p - cc))
        if pa <= pb and pa <= pc do
            return left
        if pb <= pc do
            return up
        return upLeft
    }

    // Expand 1, 2, 4 bits to 8 bits
    mtd expand8(width, height: u32, output: [*] u8)
    {
        let rowStrideIn  = .getStrideIn(width)
        let rowStrideOut = .inChannels * width

        var orgDest   = output
        let srcOffset = rowStrideOut - rowStrideIn

        // 1 bit => 8 bits
        if .header.bitDepth == 1
        {
            let mul = .paletteCount ? 1 : 255
            for height
            {
                var dest = orgDest
                orgDest += rowStrideOut
                var src = dest + srcOffset
                var k   = rowStrideOut
                while k >= 8
                {
                    let c = src[0]
                    dest[0] = cast #bit (u8) ((c >> 7) * mul)
                    dest[1] = cast #bit (u8) (((c >> 6) & 1) * mul)
                    dest[2] = cast #bit (u8) (((c >> 5) & 1) * mul)
                    dest[3] = cast #bit (u8) (((c >> 4) & 1) * mul)
                    dest[4] = cast #bit (u8) (((c >> 3) & 1) * mul)
                    dest[5] = cast #bit (u8) (((c >> 2) & 1) * mul)
                    dest[6] = cast #bit (u8) (((c >> 1) & 1) * mul)
                    dest[7] = cast #bit (u8) ((c & 1) * mul)
                    dest += 8
                    src += 1
                    k -= 8
                }
                if k > 0
                {
                    let c = src[0]
                    dest[0] = cast #bit (u8) ((c >> 7) * mul)
                    if k > 1 do
                        dest[1] = cast #bit (u8) (((c >> 6) & 1) * mul)
                    if k > 2 do
                        dest[2] = cast #bit (u8) (((c >> 5) & 1) * mul)
                    if k > 3 do
                        dest[3] = cast #bit (u8) (((c >> 4) & 1) * mul)
                    if k > 4 do
                        dest[4] = cast #bit (u8) (((c >> 3) & 1) * mul)
                    if k > 5 do
                        dest[5] = cast #bit (u8) (((c >> 2) & 1) * mul)
                    if k > 6 do
                        dest[6] = cast #bit (u8) (((c >> 1) & 1) * mul)
                }
            }
        }

        // 2 bits => 8 bits
        elif .header.bitDepth == 2
        {
            let mul = .paletteCount ? 1 : 85
            for height
            {
                var dest = orgDest
                orgDest += rowStrideOut
                var src = dest + srcOffset
                var k   = rowStrideOut
                while k >= 2
                {
                    let c = src[0]
                    dest[0] = cast #bit (u8) ((c >> 6) * mul)
                    dest[1] = cast #bit (u8) (((c >> 4) & 3) * mul)
                    dest[2] = cast #bit (u8) (((c >> 2) & 3) * mul)
                    dest[3] = cast #bit (u8) ((c & 3) * mul)
                    dest += 4
                    src += 1
                    k -= 4
                }
                if k > 0
                {
                    let c = src[0]
                    dest[0] = cast #bit (u8) ((c >> 6) * mul)
                    if k > 1 do
                        dest[1] = cast #bit (u8) (((c >> 4) & 3) * mul)
                    if k > 2 do
                        dest[2] = cast #bit (u8) (((c >> 2) & 3) * mul)
                }
            }
        }

        // 4 bits => 8 bits
        elif .header.bitDepth == 4
        {
            let mul = .paletteCount ? 1 : 17
            for height
            {
                var dest = orgDest
                orgDest += rowStrideOut
                var src = dest + srcOffset
                var k   = rowStrideOut
                while k >= 2
                {
                    let c = src[0]
                    dest[0] = cast #bit (u8) ((c >> 4) * mul)
                    dest[1] = cast #bit (u8) ((c & 15) * mul)
                    dest += 2
                    src += 1
                    k -= 2
                }
                if k > 0
                {
                    dest[0] = cast #bit (u8) ((src[0] >> 4) * mul)
                }
            }
        }
    }

    mtd getStrideIn(width: u32)->u64
    {
        if .header.bitDepth < 8 do
            return cast(u64) ((.inChannels * width * .header.bitDepth) + 7) / 8
        return cast(u64) .inChannels * width
    }

    mtd defilter8(width, height: u32, input, output: [*] u8)
    {
        let rowStrideIn  = .getStrideIn(width)
        let rowStrideOut = cast(u64) .inChannels * width
        let destOffset   = rowStrideOut - rowStrideIn

        var src  = input
        var dest = output
        if !.orderDefilter do
            dest += rowStrideOut * (height - 1)

        // A temporary line "before" the first one, if filter needs one
        var tmpRow: Array'u8
        tmpRow.resize(rowStrideOut)
        Memory.clear(tmpRow.buffer, rowStrideOut)
        var up = tmpRow.buffer

        let nk = rowStrideIn - .inChannels
        for height
        {
            let filter = cast(RowFilter) src[0]
            src += 1

            var destOff = dest + destOffset
            switch filter
            {
            case None:
                Memory.copy(destOff, src, rowStrideIn)

            case Sub:
                for i in .inChannels do
                    destOff[i] = src[i]
                for k in nk do
                    destOff[.inChannels + k] = cast #bit (u8) (src[.inChannels + k] + #wrap destOff[k])

            case Up:
                for k in rowStrideIn do
                    destOff[k] = cast #bit (u8) (src[k] + #wrap up[k])

            case Average:
                for i in .inChannels
                {
                    let avg = up[i] >> 1
                    destOff[i] = cast #bit (u8) (src[i] + #wrap avg)
                }
                for k in nk
                {
                    let avg = cast(u8) ((cast(u16) up[.inChannels + k] + cast(u16) destOff[k]) >> 1)
                    destOff[.inChannels + k] = cast #bit (u8) (src[.inChannels + k] + #wrap avg)
                }

            case Paeth:
                for i in .inChannels
                {
                    let paeth = cast(u32) filterPaeth(0, up[i], 0)
                    destOff[i] = cast #bit (u8) (src[i] + paeth)
                }
                for k in nk
                {
                    let paeth = cast(u32) filterPaeth(destOff[k], up[.inChannels + k], up[k])
                    destOff[.inChannels + k] = cast #bit (u8) (src[.inChannels + k] + #wrap paeth)
                }
            }

            src += rowStrideIn
            up = destOff
            if .orderDefilter do
                dest += rowStrideOut
            else do
                dest -= rowStrideOut
        }
    }

    #[Swag.Overload]
    mtd defilter(width, height: u32, input, output: [*] u8)
    {
        if .header.bitDepth < 8
        {
            .defilter8(width, height, input, output)
            .expand8(width, height, output)
            return
        }

        if .header.bitDepth == 8
        {
            .defilter8(width, height, input, output)
            return
        }
    }

    mtd adam7()
    {
        var tmpArr: Array'u8
        var input   = .dcmpBuf.buffer
        for p in 7
        {
            let x = (.header.width - Adam7OrigX[p] + Adam7SpacingX[p] - 1) / Adam7SpacingX[p]
            let y = (.header.height - Adam7OrigY[p] + Adam7SpacingY[p] - 1) / Adam7SpacingY[p]
            if x <= 0 or y <= 0 do
                continue

            tmpArr.resize(cast(u64) (x * y * .inChannels))
            Memory.clear(tmpArr.buffer, tmpArr.count)

            .defilter(cast(u32) x, cast(u32) y, input, tmpArr.buffer)

            var t = tmpArr.buffer
            for j in y
            {
                for i in x
                {
                    let out_y   = .header.height - ((j * Adam7SpacingY[p]) + Adam7OrigY[p]) - 1
                    let out_x   = (i * Adam7SpacingX[p]) + Adam7OrigX[p]
                    var out_off = out_y * .header.width * .inChannels
                    out_off += out_x * .inChannels
                    for z in .inChannels do
                        .dstPixels[out_off + z] = t[z]
                    t += .inChannels
                }
            }

            let stride = (.getStrideIn(cast(u32) x) + 1) * y
            input += stride
        }
    }

    #[Swag.Overload]
    mtd defilter()
    {
        if .header.interlaceMethod != .Adam7
        {
            .orderDefilter = false
            .defilter(.header.width, .header.height, .dcmpBuf.buffer, .dstPixels)
        }
        else
        {
            .orderDefilter = true
            .adam7()
        }
    }

    mtd doIt(buffer: const [..] u8)->Image throw
    {
        .stream.init(buffer)

        var signature: u64 = undefined
        .stream.peekBytes(#sizeof(signature), cast([*] u8) &signature)
        if signature != 0x0a1a0a0d474e5089 do
            throw InvalidFormatError{"not a png"}

        var result:   retval
        var iDataBuf: Array'u8
        var ch:       ChunkHeader = undefined

        var firstChunk = true
        var seenIEND   = false
        var seenIDAT   = false
        var chIsHere   = false

        while !seenIEND
        {
            // Peek a chunk header only if it's not already done
            if !chIsHere
            {
                .stream.peek32Be(cast([*] u8) &ch.length)
                .stream.peek32(cast([*] u8) &ch.type)
            }

            chIsHere = false
            if .stream.eof do
                throw InvalidFormatError{"invalid chunk"}

            switch ch.type
            {
            case IHDR:
                if !firstChunk do
                    throw InvalidFormatError{"invalid first chunk"}
                .readHeader(ch)
                if .decOptions.decodePixels
                {
                    .dstPixelsSize = .header.width * .header.height * .inChannels
                    .dstPixels     = Memory.alloc(.dstPixelsSize)
                }

            case TRNS:
                .seenTRSN = true
                .outChannels += 1 // transparency mask, add an alpha channel
                .chunkTRNS = .readChunk(ch)

            case BKGD:
                .seenBKGD = true
                discard .readChunk(ch)

            case IEND:
                seenIEND = true
                discard .readChunk(ch)

            case IDAT:
                if seenIDAT do
                    throw InvalidFormatError{"invalid IDAT chunk"}
                while ch.type == IDAT
                {
                    let chunk = .readChunk(ch)
                    iDataBuf.add(chunk.datas)
                    .stream.peek32Be(cast([*] u8) &ch.length)
                    .stream.peek32(cast([*] u8) &ch.type)
                    if .stream.eof do
                        throw InvalidFormatError{"invalid chunk"}
                }

                chIsHere, seenIDAT = true

            case PLTE:
                if seenIDAT do
                    throw InvalidFormatError{"invalid PLTE chunk"}
                let chunk = .readChunk(ch)
                let len   = @countof(chunk.datas)
                if len % 3 != 0 or len > 768 do
                    throw InvalidFormatError{"invalid PLTE chunk"}
                .seenPLTE = true
                for var i = 0; i < len; i += 3
                {
                    let col = (cast(u32) chunk.datas[i] << 16) | (cast(u32) chunk.datas[i + 1] << 8) | chunk.datas[i + 2]
                    .palette[i / 3] = col
                }

                .paletteCount = cast(u32) (len / 3)

            case TEXt:
                let chunk = .readChunk(ch)
                if .decOptions.decodeMetaDatas
                {
                    let len = @strlen(chunk.datas)
                    if len > 1
                    {
                        var md: ImageMetaData
                        md.tag = cast(string) chunk.datas[ until len]
                        if len != @countof(chunk.datas) - 1 do
                            md.value = cast(string) chunk.datas[len + 1 to ]
                        result.metaDatas.add(md)
                    }
                }

            default:
                discard .readChunk(ch)
            }

            firstChunk = false
        }

        if !seenIDAT do
            throw InvalidFormatError{"missing IDAT chunk"}
        if .header.colorType & .Paletted and !.seenPLTE do
            throw InvalidFormatError{"missing PLTE chunk"}

        // Initialize image depending on number of wanted channels
        if .outChannels == 4 do
            result.init(cast(s32) .header.width, cast(s32) .header.height, PixelFormat.RGBA8)
        else do
            result.init(cast(s32) .header.width, cast(s32) .header.height, PixelFormat.RGB8)

        // We are done if user does not want pixels
        if !.decOptions.decodePixels do
            return result

        // deflate content
        var dec: Compress.ZLib
        .dcmpBuf = dec.decompress(iDataBuf.toSlice())

        // defilter content
        .defilter()

        // If source == wanted channels, then we are done, dstPixels already contains everything
        result.allocPixels()
        if .inChannels == .outChannels
        {
            Memory.copy(result.pixels.buffer, .dstPixels, result.size)
            Memory.free(.dstPixels, .dstPixelsSize)
            return result
        }

        .convert(&result)
        return result
    }

    mtd convert(result: *Image)
    {
        defer Memory.free(.dstPixels, .dstPixelsSize)

        // Paletted, no transparency
        if .inChannels == 1 and .seenPLTE and !.seenTRSN
        {
            var pm = {.dstPixels, dec: me}
            result.visitPixels(&pm)
            {
                let params = cast(*#decltype(pm)) userData
                var src    = params.dstPixels
                src += x + y * image.width
                let pal   = src[0]
                let color = params.dec.palette[pal]
                pix[0] = cast() ((color >> 16) & 0xFF)
                pix[1] = cast() ((color >> 8) & 0xFF)
                pix[2] = cast() (color & 0xFF)
                if image.bpp == 32 do
                    pix[3] = 255
            }
        }

        // Paletted, transparency
        elif .inChannels == 1 and .seenPLTE and .seenTRSN
        {
            var pm = {.dstPixels, dec: me}
            result.visitPixels(&pm)
            {
                let params = cast(*#decltype(pm)) userData
                var src    = params.dstPixels
                src += x + y * image.width
                let pal   = src[0]
                let color = params.dec.palette[pal]
                pix[0] = cast() ((color >> 16) & 0xFF)
                pix[1] = cast() ((color >> 8) & 0xFF)
                pix[2] = cast() (color & 0xFF)
                pix[3] = pal < @countof(params.dec.chunkTRNS.datas) ? params.dec.chunkTRNS.datas[pal] : 255
            }
        }

        // Grayscale without alpha channel, no transparency
        elif .inChannels == 1 and !.seenTRSN
        {
            result.visitPixels(&.dstPixels)
            {
                var src = dref cast([*] [*] u8) userData
                src += x + y * image.width
                pix[0] = src[0]
                pix[1] = src[0]
                pix[2] = src[0]
                if image.bpp == 32 do
                    pix[3] = 255
            }
        }

        // Grayscale without alpha channel, but transparency
        elif .inChannels == 1 and .seenTRSN
        {
            var key   = .chunkTRNS.datas[1]
            let scale = .header.bitDepth == 1 ? 255'u8 : .header.bitDepth == 2 ? 85'u8 : 17'u8
            key *= scale
            var pm = {.dstPixels, key}
            result.visitPixels(&pm)
            {
                let params = cast(*#decltype(pm)) userData
                var src    = params.dstPixels
                src += x + y * image.width
                pix[0] = src[0]
                pix[1] = src[0]
                pix[2] = src[0]
                pix[3] = src[0] == params.key ? 0 : 255
            }
        }

        // Grayscale with alpha channel
        elif .inChannels == 2
        {
            result.visitPixels(&.dstPixels)
            {
                var src = dref cast([*] [*] u8) userData
                src += (x * 2) + (y * image.width * 2)
                pix[0] = src[0]
                pix[1] = src[0]
                pix[2] = src[0]
                if image.bpp == 32 do
                    pix[3] = src[1]
            }
        }

        // Color without alpha but transparency
        elif .inChannels == 3 and .seenTRSN
        {
            let key  = cast(const [*] u8) @dataof(.chunkTRNS.datas)
            let key0 = key[1]
            let key1 = key[3]
            let key2 = key[5]
            var pm   = {.dstPixels, key0, key1, key2}
            result.visitPixels(&pm)
            {
                let params = cast(*#decltype(pm)) userData
                var src    = params.dstPixels
                src += (x * 3) + (y * image.width * 3)
                pix[0] = src[0]
                pix[1] = src[1]
                pix[2] = src[2]
                if src[0] == params.key0 and src[1] == params.key1 and src[2] == params.key2 do
                    pix[3] = 0
                else do
                    pix[3] = 255
            }
        }
    }
}

public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".png"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        .decOptions = options
        return .doIt(buffer)
    }
}
