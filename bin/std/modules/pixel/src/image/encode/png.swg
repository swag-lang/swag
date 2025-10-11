/*
    :LICENCE
    This code is based on the libspng library by Randy (https://libspng.org/)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Png
using Core, Core.Errors

private
{

    const Adam7StartX: [7] u32 = [0, 4, 0, 2, 0, 1, 0]
    const Adam7StartY: [7] u32 = [0, 0, 4, 0, 2, 0, 1]
    const Adam7DeltaX: [7] u32 = [8, 8, 4, 4, 2, 2, 1]
    const Adam7DeltaY: [7] u32 = [8, 8, 8, 4, 4, 2, 2]

    struct RowInfo
    {
        scanlineIdx:     u32
        rowNum:          u32
        pass:            s32
        filter:          u8
    }

    #[Swag.EnumFlags]
    enum ColorType: u8
    {
        Paletted   = 1
        Color      = 2
        Alpha      = 4
        AlphaColor = Alpha | Color
    }

    struct EncodeFlags
    {
        filterChoice:     FilterChoice
        interlace:        bool
        sameLayout:       bool
        toBigEndian:      bool
        progressive:      bool
        finalize:         bool
    }

    enum Filter
    {
        None
        Sub
        Up
        Average
        Paeth
    }

    #[Swag.EnumFlags]
    enum FilterChoice
    {
        Disabled = 0
        None     = 8
        Sub      = 16
        Up       = 32
        Average  = 64
        Paeth    = 128
        All      = (8 | 16 | 32 | 64 | 128)
    }

    const PngSignature: [8] u8 = [137, 80, 78, 71, 13, 10, 26, 10]
    const TypeIHdr:     [4] u8 = ['I', 'H', 'D', 'R']
    const TypeIDat:     [4] u8 = ['I', 'D', 'A', 'T']
    const TypeIEnd:     [4] u8 = ['I', 'E', 'N', 'D']
    const TypeTEXt:     [4] u8 = ['t', 'E', 'X', 't']

    struct SubImage
    {
        width:             u32
        height:            u32
        outWidth:          u32
        scanlineWidth:     u32
    }

    struct Ihdr
    {
        width:                 u32
        height:                u32
        bitDepth:              u8
        colorType:             ColorType = undefined
        compressionMethod:     u8
        filterMethod:          u8
        interlaceMethod:       u8
    }

    struct ChunkBitfield
    {
        ihdr:        bool
        plte:        bool
        chrm:        bool
        iccp:        bool
        gama:        bool
        sbit:        bool
        srgb:        bool
        text:        bool
        bkgd:        bool
        hist:        bool
        trns:        bool
        phys:        bool
        splt:        bool
        time:        bool
        offs:        bool
        exif:        bool
        unknown:     bool
    }

    struct Chunk
    {
        offset:     u64
        length:     u32
        type:       [4] u8
        crc:        u32
    }
}

#[Swag.Opaque]
public struct Encoder
{
    stream:                  *ConcatBuffer
    stored:                  ChunkBitfield
    ihdr:                    Ihdr
    currentChunk:            Chunk
    tmpBuf:                  [256] u8
    writePtr:                [*] u8
    crc:                     Hash.Crc32
    subimage:                [7] SubImage
    widestPass:              s32
    lastPass:                s32
    bytesPerPixel:           u32
    scanlineBuf:             [*] u8
    prevScanlineBuf:         [*] u8
    row_buf:                 [*] u8
    filteredScanlineBuf:     [*] u8
    filteredScanline:        [*] u8
    scanline:                [*] u8
    prevScanline:            [*] u8
    encodeFlags:             EncodeFlags
    rowInfo:                 RowInfo
    pixelSize:               u32
    bufferToCompress:        Array'u8
    swapRB:                  bool
    userOptions:             EncodeOptions
}

impl Encoder
{
    #[Swag.Overload]
    func writeU32(dst: *u8, val: u32)
    {
        let v = Math.nativeToBigEndian(val)
        dref cast(*u32) dst = v
    }

    mtd writeData(buf: const [*] u8, bytes: u32)
    {
        .stream.addBytes(@mkslice(buf, bytes))
    }

    #[Swag.Overload]
    mtd writeU32(val: u32)
    {
        var v = Math.nativeToBigEndian(val)
        .stream.addBytes(@mkslice(cast(const [*] u8) &v, 4))
    }

    mtd writeHeader(chunkType: const [4] u8, chunkLength: u64)->[*] u8
    {
        let total = chunkLength + 8 + 4
        .stream.grow(total)

        .currentChunk.length = cast(u32) chunkLength
        .writeU32(.currentChunk.length)

        Memory.copy(&.currentChunk.type, chunkType, 4)
        .writeData(.currentChunk.type, 4)

        .crc.init()
        .crc.update(null)
        .crc.update(chunkType)
        .currentChunk.crc = .crc.crc

        let dataPtr = .stream.currentSP
        .stream.moveSeek(chunkLength)

        return dataPtr
    }

    mtd finishChunk()
    {
        .crc.update(@mkslice(.writePtr, .currentChunk.length))
        .currentChunk.crc = .crc.crc
        .writeU32(.currentChunk.crc)
    }

    mtd writeChunk(chunkType: const [4] u8, buf: const *u8, length: u64)
    {
        .writePtr = .writeHeader(chunkType, length)
        if length do
            Memory.copy(.writePtr, buf, length)
        .finishChunk()
    }

    mtd writeChunksBeforeIDat() throw
    {
        .writeData(PngSignature, 8)

        var dst = &.tmpBuf[0]
        writeU32(dst, .ihdr.width)
        writeU32(dst + 4, .ihdr.height)
        dst[8]  = .ihdr.bitDepth
        dst[9]  = cast(u8) .ihdr.colorType
        dst[10] = .ihdr.compressionMethod
        dst[11] = .ihdr.filterMethod
        dst[12] = .ihdr.interlaceMethod
        .writeChunk(TypeIHdr, dst, 13)
    }

    mtd writeMetaDatas(image: Image) throw
    {
        foreach md in image.metaDatas
        {
            if !md.tag.count or !md.value.count do
                continue
            .writePtr = .writeHeader(TypeTEXt, md.tag.count + 1 + md.value.count)
            Memory.copy(.writePtr, md.tag.buffer, md.tag.count)
            .writePtr[md.tag.count] = 0
            Memory.copy(.writePtr + md.tag.count + 1, md.value.buffer, md.value.count)
            .finishChunk()
        }
    }

    mtd calculateScanlineWidth(width: u32)->u32 throw
    {
        if !width do
            throw UnsupportedFormatError{"SPNG_EINTERNAL"}

        var res = cast(u64) .numChannels() * .ihdr.bitDepth
        if res > Swag.U64.Max / width do
            throw UnsupportedFormatError{"SPNG_EOVERFLOW"}
        res = res * width

        res += 15
        if res < 15 do
            throw UnsupportedFormatError{"SPNG_EOVERFLOW"}
        res /= 8
        if res > Swag.U32.Max do
            throw UnsupportedFormatError{"SPNG_EOVERFLOW"}

        return cast(u32) res
    }

    mtd numChannels()->s32
    {
        switch .ihdr.colorType
        {
        case Color:
            return 3
        case AlphaColor:
            return 4
        }

        return 0
    }

    mtd calculateSubImages() throw
    {
        if .ihdr.interlaceMethod == 1
        {
            .subimage[0].width  = (.ihdr.width + 7) >> 3
            .subimage[0].height = (.ihdr.height + 7) >> 3
            .subimage[1].width  = (.ihdr.width + 3) >> 3
            .subimage[1].height = (.ihdr.height + 7) >> 3
            .subimage[2].width  = (.ihdr.width + 3) >> 2
            .subimage[2].height = (.ihdr.height + 3) >> 3
            .subimage[3].width  = (.ihdr.width + 1) >> 2
            .subimage[3].height = (.ihdr.height + 3) >> 2
            .subimage[4].width  = (.ihdr.width + 1) >> 1
            .subimage[4].height = (.ihdr.height + 1) >> 2
            .subimage[5].width  = .ihdr.width >> 1
            .subimage[5].height = (.ihdr.height + 1) >> 1
            .subimage[6].width  = .ihdr.width
            .subimage[6].height = .ihdr.height >> 1
        }
        else
        {
            .subimage[0].width  = .ihdr.width
            .subimage[0].height = .ihdr.height
        }

        for i in 6
        {
            if .subimage[i].width == 0 or .subimage[i].height == 0 do
                continue

            .subimage[i].scanlineWidth = .calculateScanlineWidth(.subimage[i].width)
            if .subimage[.widestPass].scanlineWidth < .subimage[i].scanlineWidth do
                .widestPass = i
            .lastPass = i
        }
    }

    mtd finishIDat() throw
    {
        var zlib: Compress.ZLib
        let cmp   = zlib.compress(.bufferToCompress.toSlice(), .userOptions.compressionLevel)

        .writePtr = .writeHeader(TypeIDat, cmp.count)
        Memory.copy(.writePtr, cmp.buffer, cmp.count)
        .bufferToCompress.count = 0

        .finishChunk()
    }

    mtd writeIDatBytes(sl: [*] u8, len: u32) throw
    {
        .bufferToCompress.add(@mkslice(sl, len))
    }

    mtd encodeRow(row: [*] u8)->bool throw
    {
        let pass = .rowInfo.pass

        if !.ihdr.interlaceMethod or pass == 6 do
            return .encodeScanLine(row)

        let bitDepth = .ihdr.bitDepth
        if bitDepth < 8
        {
            let samplesPerByte = 8 / bitDepth
            let mask           = cast(u8) ((1 << bitDepth) - 1)
            let initialShift   = 8 - bitDepth
            var shiftAmount    = initialShift

            let row_uc = row
            Memory.clear(.scanline, cast(u64) .subimage[pass].scanlineWidth)

            for k in .subimage[pass].width
            {
                let ioffset = Adam7StartX[pass] + k * Adam7DeltaX[pass]

                var sample = row_uc[ioffset / samplesPerByte]
                sample = sample >> (initialShift - ioffset * bitDepth % 8)
                sample = sample & mask
                sample = sample << shiftAmount

                .scanline[0] |= sample

                shiftAmount -= bitDepth

                if shiftAmount > 7
                {
                    shiftAmount = initialShift
                    .scanline += 1
                }
            }

            return .encodeScanLine(.scanline)
        }

        for k in .subimage[pass].width
        {
            let ioffset = (Adam7StartX[pass] + k * Adam7DeltaX[pass]) * .pixelSize
            Memory.copy(.scanline + k * .pixelSize, row + ioffset, .pixelSize)
        }

        return .encodeScanLine(.scanline)
    }

    mtd getBestFilter(size: u32)->Filter
    {
        if .encodeFlags.filterChoice == .Disabled do
            return .None

        let chl = cast(s32) .encodeFlags.filterChoice
        if !(chl & (chl - 1))
        {
            for i in 5
            {
                if chl == 1 << (i + 3) do
                    return cast() i
            }
        }

        var bestFilter   = 0
        var bestScore    = Swag.S32.Max
        var filterScores = [Swag.S32.Max, Swag.S32.Max, Swag.S32.Max, Swag.S32.Max, Swag.S32.Max]

        var sum: s32
        for i in 5
        {
            let flag = 1 << (i + 3)
            if !(chl & flag) do
                continue

            sum             = .filterSum(size, cast(Filter) i)
            filterScores[i] = Math.abs(sum)
            if filterScores[i] < bestScore
            {
                bestScore  = filterScores[i]
                bestFilter = i
            }
        }

        return cast() bestFilter
    }

    func paeth(a, b, c: s32)->u32
    {
        let p  = cast(s16) (a + b - c)
        let pa = Math.abs(cast(s16) (p - a))
        let pb = Math.abs(cast(s16) (p - b))
        let pc = Math.abs(cast(s16) (p - c))

        if pa <= pb and pa <= pc do
            return a
        elif pb <= pc do
            return b
        return c
    }

    mtd filterSum(size: u32, filter: Filter)->s32
    {
        if size > Swag.S32.Max / 128 do
            return Swag.S32.Max

        var sum: s32
        var a, b, c: s32

        for i in size
        {
            if i >= .bytesPerPixel
            {
                a = .scanline[i - .bytesPerPixel]
                b = .prevScanline[i]
                c = .prevScanline[i - .bytesPerPixel]
            }
            else
            {
                a = 0
                b = .prevScanline[i]
                c = 0
            }

            var x = cast(s32) .scanline[i]

            switch filter
            {
            case None:
                break
            case Sub:
                x = x - a
            case Up:
                x = x - b
            case Average:
                let avg = (a + b) / 2
                x = x - avg
            case Paeth:
                x = x - paeth(a, b, c)
            }

            sum += 128 - Math.abs(x - 128)
        }

        return sum
    }

    mtd filterScanline(filtered: [*] u8, size: u32, filter: Filter)
    {
        for i in size
        {
            var a, b, c: s32
            if i >= .bytesPerPixel
            {
                a = .scanline[i - .bytesPerPixel]
                b = .prevScanline[i]
                c = .prevScanline[i - .bytesPerPixel]
            }
            else
            {
                a = 0
                b = .prevScanline[i]
                c = 0
            }

            var x = cast(s32) .scanline[i]

            switch filter
            {
            case Sub:
                x = x - a
            case Up:
                x = x - b
            case Average:
                let avg = (a + b) / 2
                x = x - avg
            case Paeth:
                x = x - paeth(a, b, c)
            }

            filtered[i] = cast #bit (u8) x
        }
    }

    mtd encodeScanLine(sl: [*] u8)->bool throw
    {
        let pass = .rowInfo.pass

        let scanlineWidth = .subimage[pass].scanlineWidth
        if sl != .scanline
        {
            if .swapRB and .bytesPerPixel == 3
            {
                for var i = 0; i < scanlineWidth - 1; i += 3
                {
                    .scanline[i]     = sl[i + 2]
                    .scanline[i + 1] = sl[i + 1]
                    .scanline[i + 2] = sl[i]
                }
            }
            elif .swapRB and .bytesPerPixel == 4
            {
                for var i = 0; i < scanlineWidth - 1; i += 4
                {
                    .scanline[i]     = sl[i + 2]
                    .scanline[i + 1] = sl[i + 1]
                    .scanline[i + 2] = sl[i]
                    .scanline[i + 3] = sl[i + 3]
                }
            }
            else
            {
                Memory.copy(.scanline, sl, cast(u64) scanlineWidth - 1)
            }
        }

        let requiresPrevious = .encodeFlags.filterChoice & (.Up | .Average | .Paeth)
        if !.rowInfo.scanlineIdx and requiresPrevious do
            Memory.clear(.prevScanline, cast(u64) scanlineWidth)

        var filtered_sl = .filteredScanline
        let filter      = .getBestFilter(scanlineWidth - 1)
        if filter == .None do
            filtered_sl = .scanline
        filtered_sl -= 1
        filtered_sl[0] = cast(u8) filter
        filtered_sl += 1

        if filter do
            .filterScanline(filtered_sl, scanlineWidth - 1, filter)

        .writeIDatBytes(filtered_sl - 1, scanlineWidth)

        swap(&.prevScanline, &.scanline)
        if .updateRowInfo()
        {
            .finishIDat()
            return true
        }

        return false
    }

    mtd updateRowInfo()->bool
    {
        let interlacing = .ihdr.interlaceMethod
        let ri          = &.rowInfo
        var sub         = .subimage

        if ri.scanlineIdx == sub[ri.pass].height - 1
        {
            if ri.pass == .lastPass do
                return true

            ri.scanlineIdx = 0
            ri.pass += 1

            while (!sub[ri.pass].width or !sub[ri.pass].height) and (ri.pass < .lastPass) do
                ri.pass += 1
        }
        else
        {
            ri.rowNum += 1
            ri.scanlineIdx += 1
        }

        if interlacing do
            ri.rowNum = Adam7StartY[ri.pass] + ri.scanlineIdx * Adam7DeltaY[ri.pass]

        return false
    }
}

public struct EncodeOptions
{
    compressionLevel     = Compress.Deflate.CompressionLevel.Default
    filtering            = true
}

public impl IImageEncoder for Encoder
{
    mtd impl canEncode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".png"
    }

    mtd impl encode(result: *ConcatBuffer, image: Image, options: any) throw
    {
        if @dataof(options) != null
        {
            Debug.assert(@kindof(options) == EncodeOptions)
            .userOptions = dref cast(*EncodeOptions) @dataof(options)
        }

        // unsupported pixel format
        if image.pf.channels() < 3 or image.pf.bitDepth() != 8 do
            throw UnsupportedFormatError{"unsupported pixel format"}
        if image.width == 0 or image.height == 0 do
            throw UnsupportedFormatError{"unsupported size"}

        .swapRB = image.pf == .BGR8 or image.pf == .BGRA8

        .stream = result

        .stored.ihdr    = true
        .ihdr.width     = image.width
        .ihdr.height    = image.height
        .ihdr.bitDepth  = 8
        .ihdr.colorType = .Color
        if image.bpp == 32 do
            .ihdr.colorType |= .Alpha

        .writeChunksBeforeIDat()
        .writeMetaDatas(image)
        .calculateSubImages()

        .bytesPerPixel = .numChannels() * (.ihdr.bitDepth / 8)
        var scanlineBufSize = .subimage[.widestPass].scanlineWidth
        scanlineBufSize += 32

        .scanlineBuf         = Memory.alloc(cast(u64) scanlineBufSize)
        .prevScanlineBuf     = Memory.alloc(cast(u64) scanlineBufSize)
        .filteredScanlineBuf = Memory.alloc(cast(u64) scanlineBufSize)
        defer Memory.free(.scanlineBuf, cast(u64) scanlineBufSize)
        defer Memory.free(.prevScanlineBuf, cast(u64) scanlineBufSize)
        defer Memory.free(.filteredScanlineBuf, cast(u64) scanlineBufSize)

        .scanline         = .scanlineBuf + 16
        .prevScanline     = .prevScanlineBuf + 16
        .filteredScanline = .filteredScanlineBuf + 16

        .pixelSize = image.bpp
        if .userOptions.filtering and .userOptions.compressionLevel != .NoCompression do
            .encodeFlags.filterChoice = .All

        for
        {
            let ioffset = (image.height - .rowInfo.rowNum - 1) * image.width * image.bpp8
            if .encodeRow(image.pixels.buffer + ioffset) do
                break
        }

        discard .writeHeader(TypeIEnd, 0)
        .finishChunk()
    }
}
