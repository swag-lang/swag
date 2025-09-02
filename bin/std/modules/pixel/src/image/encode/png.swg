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
    writePtr:                ^u8
    crc:                     Hash.Crc32
    subimage:                [7] SubImage
    widestPass:              s32
    lastPass:                s32
    bytesPerPixel:           u32
    scanlineBuf:             ^u8
    prevScanlineBuf:         ^u8
    row_buf:                 ^u8
    filteredScanlineBuf:     ^u8
    filteredScanline:        ^u8
    scanline:                ^u8
    prevScanline:            ^u8
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

    mtd writeData(buf: const ^u8, bytes: u32)
    {
        me.stream.addBytes(@mkslice(buf, bytes))
    }

    #[Swag.Overload]
    mtd writeU32(val: u32)
    {
        var v = Math.nativeToBigEndian(val)
        me.stream.addBytes(@mkslice(cast(const ^u8) &v, 4))
    }

    mtd writeHeader(chunkType: const [4] u8, chunkLength: u64)->^u8
    {
        let total = chunkLength + 8 + 4
        me.stream.grow(total)

        me.currentChunk.length = cast(u32) chunkLength
        me.writeU32(me.currentChunk.length)

        Memory.copy(&me.currentChunk.type, chunkType, 4)
        me.writeData(me.currentChunk.type, 4)

        me.crc.init()
        me.crc.update(null)
        me.crc.update(chunkType)
        me.currentChunk.crc = me.crc.crc

        let dataPtr = me.stream.currentSP
        me.stream.moveSeek(chunkLength)

        return dataPtr
    }

    mtd finishChunk()
    {
        me.crc.update(@mkslice(me.writePtr, me.currentChunk.length))
        me.currentChunk.crc = me.crc.crc
        me.writeU32(me.currentChunk.crc)
    }

    mtd writeChunk(chunkType: const [4] u8, buf: const *u8, length: u64)
    {
        me.writePtr = me.writeHeader(chunkType, length)
        if length:
            Memory.copy(me.writePtr, buf, length)
        me.finishChunk()
    }

    mtd writeChunksBeforeIDat() throw
    {
        me.writeData(PngSignature, 8)

        var dst = &me.tmpBuf[0]
        writeU32(dst, me.ihdr.width)
        writeU32(dst + 4, me.ihdr.height)
        dst[8]  = me.ihdr.bitDepth
        dst[9]  = cast(u8) me.ihdr.colorType
        dst[10] = me.ihdr.compressionMethod
        dst[11] = me.ihdr.filterMethod
        dst[12] = me.ihdr.interlaceMethod
        me.writeChunk(TypeIHdr, dst, 13)
    }

    mtd writeMetaDatas(image: Image) throw
    {
        foreach md in image.metaDatas
        {
            if !md.tag.count or !md.value.count:
                continue
            me.writePtr = me.writeHeader(TypeTEXt, md.tag.count + 1 + md.value.count)
            Memory.copy(me.writePtr, md.tag.buffer, md.tag.count)
            me.writePtr[md.tag.count] = 0
            Memory.copy(me.writePtr + md.tag.count + 1, md.value.buffer, md.value.count)
            me.finishChunk()
        }
    }

    mtd calculateScanlineWidth(width: u32)->u32 throw
    {
        if !width:
            throw UnsupportedFormatError{"SPNG_EINTERNAL"}

        var res = cast(u64) me.numChannels() * me.ihdr.bitDepth
        if res > Swag.U64.Max / width:
            throw UnsupportedFormatError{"SPNG_EOVERFLOW"}
        res = res * width

        res += 15
        if res < 15:
            throw UnsupportedFormatError{"SPNG_EOVERFLOW"}
        res /= 8
        if res > Swag.U32.Max:
            throw UnsupportedFormatError{"SPNG_EOVERFLOW"}

        return cast(u32) res
    }

    mtd numChannels()->s32
    {
        switch me.ihdr.colorType
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
        if me.ihdr.interlaceMethod == 1
        {
            me.subimage[0].width  = (me.ihdr.width + 7) >> 3
            me.subimage[0].height = (me.ihdr.height + 7) >> 3
            me.subimage[1].width  = (me.ihdr.width + 3) >> 3
            me.subimage[1].height = (me.ihdr.height + 7) >> 3
            me.subimage[2].width  = (me.ihdr.width + 3) >> 2
            me.subimage[2].height = (me.ihdr.height + 3) >> 3
            me.subimage[3].width  = (me.ihdr.width + 1) >> 2
            me.subimage[3].height = (me.ihdr.height + 3) >> 2
            me.subimage[4].width  = (me.ihdr.width + 1) >> 1
            me.subimage[4].height = (me.ihdr.height + 1) >> 2
            me.subimage[5].width  = me.ihdr.width >> 1
            me.subimage[5].height = (me.ihdr.height + 1) >> 1
            me.subimage[6].width  = me.ihdr.width
            me.subimage[6].height = me.ihdr.height >> 1
        }
        else
        {
            me.subimage[0].width  = me.ihdr.width
            me.subimage[0].height = me.ihdr.height
        }

        for i in 6
        {
            if me.subimage[i].width == 0 or me.subimage[i].height == 0:
                continue

            me.subimage[i].scanlineWidth = me.calculateScanlineWidth(me.subimage[i].width)
            if me.subimage[me.widestPass].scanlineWidth < me.subimage[i].scanlineWidth:
                me.widestPass = i
            me.lastPass = i
        }
    }

    mtd finishIDat() throw
    {
        var zlib: Compress.ZLib
        var cmp   = zlib.compress(me.bufferToCompress.toSlice(), me.userOptions.compressionLevel)

        me.writePtr = me.writeHeader(TypeIDat, cmp.count)
        Memory.copy(me.writePtr, cmp.buffer, cmp.count)
        me.bufferToCompress.count = 0

        me.finishChunk()
    }

    mtd writeIDatBytes(sl: ^u8, len: u32) throw
    {
        me.bufferToCompress.add(@mkslice(sl, len))
    }

    mtd encodeRow(row: ^u8)->bool throw
    {
        let pass = me.rowInfo.pass

        if !me.ihdr.interlaceMethod or pass == 6:
            return me.encodeScanLine(row)

        let bitDepth = me.ihdr.bitDepth
        if bitDepth < 8
        {
            let samplesPerByte = 8 / bitDepth
            let mask           = cast(u8) ((1 << bitDepth) - 1)
            let initialShift   = 8 - bitDepth
            var shiftAmount    = initialShift

            let row_uc = row
            Memory.clear(me.scanline, cast(u64) me.subimage[pass].scanlineWidth)

            for k in me.subimage[pass].width
            {
                let ioffset = Adam7StartX[pass] + k * Adam7DeltaX[pass]

                var sample = row_uc[ioffset / samplesPerByte]
                sample = sample >> (initialShift - ioffset * bitDepth % 8)
                sample = sample & mask
                sample = sample << shiftAmount

                me.scanline[0] |= sample

                shiftAmount -= bitDepth

                if shiftAmount > 7
                {
                    shiftAmount = initialShift
                    me.scanline += 1
                }
            }

            return me.encodeScanLine(me.scanline)
        }

        for k in me.subimage[pass].width
        {
            let ioffset = (Adam7StartX[pass] + k * Adam7DeltaX[pass]) * me.pixelSize
            Memory.copy(me.scanline + k * me.pixelSize, row + ioffset, me.pixelSize)
        }

        return me.encodeScanLine(me.scanline)
    }

    mtd getBestFilter(size: u32)->Filter
    {
        if me.encodeFlags.filterChoice == .Disabled:
            return .None

        let chl = cast(s32) me.encodeFlags.filterChoice
        if !(chl & (chl - 1))
        {
            for i in 5
            {
                if chl == 1 << (i + 3):
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
            if !(chl & flag):
                continue

            sum             = me.filterSum(size, cast(Filter) i)
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

        if pa <= pb and pa <= pc:
            return a
        elif pb <= pc:
            return b
        return c
    }

    mtd filterSum(size: u32, filter: Filter)->s32
    {
        if size > Swag.S32.Max / 128:
            return Swag.S32.Max

        var sum: s32
        var a, b, c: s32

        for i in size
        {
            if i >= me.bytesPerPixel
            {
                a = me.scanline[i - me.bytesPerPixel]
                b = me.prevScanline[i]
                c = me.prevScanline[i - me.bytesPerPixel]
            }
            else
            {
                a = 0
                b = me.prevScanline[i]
                c = 0
            }

            var x = cast(s32) me.scanline[i]

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

    mtd filterScanline(filtered: ^u8, size: u32, filter: Filter)
    {
        for i in size
        {
            var a, b, c: s32
            if i >= me.bytesPerPixel
            {
                a = me.scanline[i - me.bytesPerPixel]
                b = me.prevScanline[i]
                c = me.prevScanline[i - me.bytesPerPixel]
            }
            else
            {
                a = 0
                b = me.prevScanline[i]
                c = 0
            }

            var x = cast(s32) me.scanline[i]

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

    mtd encodeScanLine(sl: ^u8)->bool throw
    {
        let pass = me.rowInfo.pass

        let scanlineWidth = me.subimage[pass].scanlineWidth
        if sl != me.scanline
        {
            if me.swapRB and me.bytesPerPixel == 3
            {
                for var i = 0; i < scanlineWidth - 1; i += 3
                {
                    me.scanline[i]     = sl[i + 2]
                    me.scanline[i + 1] = sl[i + 1]
                    me.scanline[i + 2] = sl[i]
                }
            }
            elif me.swapRB and me.bytesPerPixel == 4
            {
                for var i = 0; i < scanlineWidth - 1; i += 4
                {
                    me.scanline[i]     = sl[i + 2]
                    me.scanline[i + 1] = sl[i + 1]
                    me.scanline[i + 2] = sl[i]
                    me.scanline[i + 3] = sl[i + 3]
                }
            }
            else
            {
                Memory.copy(me.scanline, sl, cast(u64) scanlineWidth - 1)
            }
        }

        let requiresPrevious = me.encodeFlags.filterChoice & (.Up | .Average | .Paeth)
        if !me.rowInfo.scanlineIdx and requiresPrevious:
            Memory.clear(me.prevScanline, cast(u64) scanlineWidth)

        var filtered_sl = me.filteredScanline
        let filter      = me.getBestFilter(scanlineWidth - 1)
        if filter == .None:
            filtered_sl = me.scanline
        filtered_sl -= 1
        filtered_sl[0] = cast(u8) filter
        filtered_sl += 1

        if filter:
            me.filterScanline(filtered_sl, scanlineWidth - 1, filter)

        me.writeIDatBytes(filtered_sl - 1, scanlineWidth)

        swap(&me.prevScanline, &me.scanline)
        if me.updateRowInfo()
        {
            me.finishIDat()
            return true
        }

        return false
    }

    mtd updateRowInfo()->bool
    {
        let interlacing = me.ihdr.interlaceMethod
        let ri          = &me.rowInfo
        var sub         = me.subimage

        if ri.scanlineIdx == sub[ri.pass].height - 1
        {
            if ri.pass == me.lastPass:
                return true

            ri.scanlineIdx = 0
            ri.pass += 1

            while (!sub[ri.pass].width or !sub[ri.pass].height) and (ri.pass < me.lastPass):
                ri.pass += 1
        }
        else
        {
            ri.rowNum += 1
            ri.scanlineIdx += 1
        }

        if interlacing:
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
            me.userOptions = dref cast(*EncodeOptions) @dataof(options)
        }

        // unsupported pixel format
        if image.pf.channels() < 3 or image.pf.bitDepth() != 8:
            throw UnsupportedFormatError{"unsupported pixel format"}
        if image.width == 0 or image.height == 0:
            throw UnsupportedFormatError{"unsupported size"}

        me.swapRB = image.pf == .BGR8 or image.pf == .BGRA8

        me.stream = result

        me.stored.ihdr    = true
        me.ihdr.width     = image.width
        me.ihdr.height    = image.height
        me.ihdr.bitDepth  = 8
        me.ihdr.colorType = .Color
        if image.bpp == 32:
            me.ihdr.colorType |= .Alpha

        me.writeChunksBeforeIDat()
        me.writeMetaDatas(image)
        me.calculateSubImages()

        me.bytesPerPixel = me.numChannels() * (me.ihdr.bitDepth / 8)
        var scanlineBufSize = me.subimage[me.widestPass].scanlineWidth
        scanlineBufSize += 32

        me.scanlineBuf         = Memory.alloc(cast(u64) scanlineBufSize)
        me.prevScanlineBuf     = Memory.alloc(cast(u64) scanlineBufSize)
        me.filteredScanlineBuf = Memory.alloc(cast(u64) scanlineBufSize)
        defer Memory.free(me.scanlineBuf, cast(u64) scanlineBufSize)
        defer Memory.free(me.prevScanlineBuf, cast(u64) scanlineBufSize)
        defer Memory.free(me.filteredScanlineBuf, cast(u64) scanlineBufSize)

        me.scanline         = me.scanlineBuf + 16
        me.prevScanline     = me.prevScanlineBuf + 16
        me.filteredScanline = me.filteredScanlineBuf + 16

        me.pixelSize = image.bpp
        if me.userOptions.filtering and me.userOptions.compressionLevel != .NoCompression:
            me.encodeFlags.filterChoice = .All

        for
        {
            let ioffset = (image.height - me.rowInfo.rowNum - 1) * image.width * image.bpp8
            if me.encodeRow(image.pixels.buffer + ioffset):
                break
        }

        discard me.writeHeader(TypeIEnd, 0)
        me.finishChunk()
    }
}
