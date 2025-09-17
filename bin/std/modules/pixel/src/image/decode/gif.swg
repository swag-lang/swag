// :LICENCE
// This code is based on "gifdec" (https://github.com/lecram/gifdec)
// The original code has been modified for Swag

#global namespace Gif
using Swag, Core, Core.Errors

private
{

    struct Entry
    {
        length:     u16
        prefix:     u16
        suffix:     u8
    }

    struct Table
    {
        bulk:         s32
        nentries:     s32
        entries:      Array'Entry
    }

    struct Palette
    {
        size:       s32
        colors:     [0x100 * 3] u8
    }

    struct Gce
    {
        delay:            u16
        tindex:           u8
        disposal:         u8
        input:            s32
        transparency:     s32
    }
}

#[Swag.Opaque]
public struct Decoder
{
    fd:              s32
    width:           u16
    height:          u16
    depth:           u16
    loopCount:       u16
    numFrames:       s32
    gce:             Gce
    palette:         *Palette
    lct:             Palette
    gct:             Palette
    fx:              u16
    fy:              u16
    fw:              u16
    fh:              u16
    bgIndex:         u8
    canvas:          [*] u8
    frame:           Array'u8
    table:           Table
    animStart:       u64
    animRemain:      u64
    reachEnd:        bool
    initDone:        bool

    streamStart:     const [*] u8
    stream:          const [*] u8
    streamLen:       u64
}

impl Decoder
{
    mtd seek(len: u64) throw
    {
        if len > me.streamLen:
            throw InvalidFormatError{"read error"}
        me.stream += len
        me.streamLen -= len
    }

    mtd read(dest: *u8, len: u64) throw
    {
        if len > me.streamLen:
            throw InvalidFormatError{"read error"}
        Memory.copy(dest, me.stream, len)
        me.stream += len
        me.streamLen -= len
    }

    #[Swag.Inline]
    mtd readU8()->u8 throw
    {
        if me.streamLen < 1:
            throw InvalidFormatError{"read error"}
        let res = me.stream[0]
        me.stream += 1
        me.streamLen -= 1
        return res
    }

    mtd readU16()->u16 throw
    {
        if me.streamLen < 2:
            throw InvalidFormatError{"read error"}
        let res = cast(u16) (me.stream[0] + (cast(u16) me.stream[1] << 8))
        me.stream += 2
        me.streamLen -= 2
        return res
    }

    mtd discardSubBlocks() throw
    {
        for
        {
            let size = me.readU8()
            if !size:
                break
            me.seek(cast() size)
        }
    }

    mtd readPlainTextExt() throw
    {
        // Discard plain text metadata.
        me.seek(13)
        // Discard plain text sub-blocks.
        me.discardSubBlocks()
    }

    mtd readGraphicControlExt() throw
    {
        // Discard block size (always 0x04)
        me.seek(1)
        let rdit = me.readU8()
        me.gce.disposal     = (rdit >> 2) & 3
        me.gce.input        = rdit & 2
        me.gce.transparency = rdit & 1
        me.gce.delay        = me.readU16()
        me.gce.tindex       = me.readU8()
        // Skip block terminator
        me.seek(1)
    }

    mtd readCommentExt() throw
    {
        me.discardSubBlocks()
    }

    mtd readApplicationExt() throw
    {
        var app_id:        [8] u8
        var app_auth_code: [3] u8

        // Discard block size (always 0x0B)
        me.seek(1)
        // Application Identifier
        me.read(app_id, 8)
        // Application Authentication Code
        me.read(app_auth_code, 3)

        if !Memory.compare(app_id, @dataof("NETSCAPE"), @countof(app_id))
        {
            // Discard block size (0x03) and constant byte (0x01).
            me.seek(2)
            me.loopCount = me.readU16()
            // Skip block terminator.
            me.seek(1)
        }
        else
        {
            me.discardSubBlocks()
        }
    }

    mtd readExt() throw
    {
        let label = me.readU8()
        switch label
        {
        case 0x01:
            me.readPlainTextExt()
        case 0xF9:
            me.readGraphicControlExt()
        case 0xFE:
            me.readCommentExt()
        case 0xFF:
            me.readApplicationExt()
        default:
            throw UnsupportedFormatError{Errors.mkString(Format.toString("unknown extension: %", label))}
        }
    }

    mtd initTable(keySize: s32)
    {
        me.table.bulk     = Math.max(1 << (keySize + 1), 0x100's32)
        me.table.nentries = (1 << keySize) + 2
        me.table.entries.resize(me.table.bulk)
        for key in 1 << keySize:
            me.table.entries[key] = {1, 0xFFF, cast(u8) key}
    }

    mtd getKey(keySize: s32, subLen, shift, byte: *u8)->u16 throw
    {
        var key       = 0'u16
        var frag_size = 0
        for var bits_read = 0; bits_read < keySize; bits_read += frag_size
        {
            let rpad = (dref shift + bits_read) % 8
            if rpad == 0
            {
                // Update byte.
                if dref subLen == 0
                {
                    dref subLen = me.readU8() // Must be nonzero!
                    if dref subLen == 0:
                        return 0x1000
                }

                dref byte = me.readU8()
                dref subLen -= 1
            }

            frag_size = Math.min(cast(s32) (keySize - bits_read), cast(s32) (8 - rpad))
            key |= cast(u16) ((dref byte) >> rpad) << bits_read
        }

        // Clear extra bits to the left
        key &= cast(u16) ((1 << keySize) - 1)
        dref shift = cast(u8) ((dref shift + keySize) % 8)
        return key
    }

    mtd addEntry(length: u16, prefix: u16, suffix: u8)->s32
    {
        if me.table.nentries == me.table.bulk
        {
            me.table.bulk *= 2
            me.table.entries.resize(cast(u64) me.table.bulk)
        }

        me.table.entries[me.table.nentries] = {length, prefix, suffix}
        me.table.nentries += 1
        if (me.table.nentries & (me.table.nentries - 1)) == 0:
            return 1
        return 0
    }

    func interlacedLineIndex(h0, y0: s32)->s32
    {
        let h = h0
        var y = y0
        var p = (h - 1) / 8 + 1
        if (y < p):
            return y * 8
        y -= p
        p = (h - 5) / 8 + 1
        if (y < p):
            return y * 8 + 4
        y -= p
        p = (h - 3) / 4 + 1
        if (y < p):
            return y * 4 + 2
        y -= p
        // pass 4
        return y * 2 + 1
    }

    mtd readImageData(interlace: u8) throw
    {
        var byte    = me.readU8()
        var keySize = cast(s32) byte
        if keySize < 2 or keySize > 8:
            throw InvalidFormatError{"invalid frame"}

        let start       = cast(u64) (me.stream - me.streamStart)
        let remainStart = me.streamLen
        me.discardSubBlocks()
        let end       = cast(u64) (me.stream - me.streamStart)
        let remainEnd = me.streamLen
        me.stream    = me.streamStart + start
        me.streamLen = remainStart

        let clear = 1 << keySize
        let stop  = clear + 1
        me.initTable(keySize)
        keySize += 1
        let initKeySize = keySize
        var subLen, shift = 0'u8
        var key         = me.getKey(keySize, &subLen, &shift, &byte)     // clear code
        var frmOff      = 0
        var ret         = 0
        let frmSize     = cast(u32) me.fw * me.fh
        var tableIsFull = 0
        var strLen      = 0'u16

        var entry: Entry
        while frmOff < frmSize
        {
            if key == clear
            {
                keySize           = initKeySize
                me.table.nentries = (1 << (keySize - 1)) + 2
                tableIsFull       = 0
            }
            elif !tableIsFull
            {
                ret = me.addEntry(cast(u16) (strLen + 1), key, entry.suffix)
                if me.table.nentries == 0x1000
                {
                    ret         = 0
                    tableIsFull = 1
                }
            }

            key = me.getKey(keySize, &subLen, &shift, &byte)
            if key == clear:
                continue
            if key == stop or key == 0x1000:
                break
            if ret == 1:
                keySize += 1
            entry  = me.table.entries.buffer[key]
            strLen = entry.length
            for i in strLen
            {
                let p = frmOff + entry.length - 1
                let x = p % me.fw
                var y = p / me.fw
                if interlace:
                    y = interlacedLineIndex(cast(s32) me.fh, y)
                me.frame[(me.fy + y) * me.width + me.fx + x] = entry.suffix
                if entry.prefix == 0xFFF:
                    break
                entry = me.table.entries.buffer[entry.prefix]
            }

            frmOff += strLen
            if key < me.table.nentries - 1 and !tableIsFull:
                me.table.entries[me.table.nentries - 1].suffix = entry.suffix
        }

        if key == stop:
            subLen = me.readU8() // Must be zero!
        me.stream    = me.streamStart + end
        me.streamLen = remainEnd
    }

    mtd readImage() throw
    {
        // Image descriptor
        me.fx = me.readU16()
        me.fy = me.readU16()
        if me.fx >= me.width or me.fy >= me.height:
            throw InvalidFormatError{"invalid frame size"}

        me.fw = me.readU16()
        me.fh = me.readU16()
        me.fw = Math.min(me.fw, cast(u16) (me.width - me.fx))
        me.fh = Math.min(me.fh, cast(u16) (me.height - me.fy))

        let fisrz     = me.readU8()
        let interlace = fisrz & 0x40

        // Ignore Sort Flag
        // Local Color Table?
        if fisrz & 0x80
        {
            /* Read LCT */
            me.lct.size = 1 << ((fisrz & 0x07) + 1)
            me.read(me.lct.colors, 3'u64 * me.lct.size)
            me.palette = &me.lct
        }
        else:
            me.palette = &me.gct

        me.readImageData(interlace)
    }

    mtd renderFrame(buffer: [*] u8, pf: PixelFormat)
    {
        var i = cast(u32) me.fy * me.width + me.fx

        if pf.hasAlpha()
        {
            for #reverse j in 0 until me.fh
            {
                for k in cast(u32) me.fw
                {
                    let index = me.frame[(me.fy + j) * cast(u32) me.width + (me.fx + k)]
                    let color = &me.palette.colors[index * 3'u32]
                    let b     = buffer + (i + k) * 4
                    b[0] = color[0]
                    b[1] = color[1]
                    b[2] = color[2]
                    b[3] = index == me.gce.tindex ? 0'u8 : 0xFF'u8
                }

                i += me.width
            }
        }
        else
        {
            for #reverse j in 0 until me.fh
            {
                for k in me.fw
                {
                    let index = me.frame[(me.fy + j) * me.width + (me.fx + k)]
                    let color = &me.palette.colors[index * 3]
                    let b     = buffer + (i + k) * 4
                    b[0] = color[0]
                    b[1] = color[1]
                    b[2] = color[2]
                }

                i += me.width
            }
        }
    }

    #[Swag.Overload]
    mtd getFrame(decodePixels: bool)->s32 throw
    {
        var sep = me.readU8()
        while sep != ','
        {
            if sep == ';':
                return 0
            if sep != '!':
                throw InvalidFormatError{"invalid format"}
            me.readExt()
            sep = me.readU8()
        }

        if decodePixels:
            me.readImage()
        return 1
    }
}

public impl Decoder
{
    // Initialize the decoder
    mtd init(buffer: const [..] u8, decodePixels: bool) throw
    {
        me.initDone = true
        me.streamStart, me.stream = @dataof(buffer)
        me.streamLen = @countof(buffer)

        // Header
        var sigver: [3] u8
        me.read(sigver, 3)
        if Memory.compare(sigver, @dataof("GIF"), 3) != 0:
            throw InvalidFormatError{"invalid signature"}

        // Version
        me.read(sigver, 3)
        if Memory.compare(sigver, @dataof("89a"), 3) != 0:
            throw InvalidFormatError{"invalid version"}

        // Width x Height
        me.width  = me.readU16()
        me.height = me.readU16()

        // FDSZ
        var fdsz: u8
        me.read(&fdsz, 1)
        if !(fdsz & 0x80):
            throw UnsupportedFormatError{"no global color table"}

        // Color space's depth
        me.depth = cast() (((fdsz >> 4) & 7) + 1)

        // Ignore sort flag
        // GCT Size
        me.gct.size = 1 << ((fdsz & 0x07) + 1)

        // Background color index
        me.bgIndex = me.readU8()

        // Aspect ratio
        discard me.readU8()

        me.read(me.gct.colors, 3'u64 * me.gct.size)
        me.palette = &me.gct

        if decodePixels
        {
            let size = cast(u64) me.width * cast(u64) me.height
            me.frame.resize(4 * size)
            Memory.set(me.frame.buffer, me.bgIndex, size)
            me.canvas = &me.frame.buffer[size]

            let bgcolor = &me.palette.colors[me.bgIndex * 3'u32]
            if bgcolor[0] or bgcolor[1] or bgcolor[2]
            {
                for i in size:
                    Memory.copy(&me.canvas[i * 3], bgcolor, 3)
            }
        }

        me.animStart  = cast(u64) (me.stream - me.streamStart)
        me.animRemain = me.streamLen

        discard me.getFrame(decodePixels)
    }

    // Create an image for the current frame
    #[Swag.Overload]
    mtd getFrame(image: *Image, decodePixels = true)
    {
        Debug.assert(me.initDone)
        image.init(me.width, me.height, me.gce.transparency ? PixelFormat.RGBA8 : PixelFormat.RGB8)
        if !decodePixels:
            return
        image.allocPixels()
    }

    // Get the next frame. Returns false if we have reached the end
    mtd nextFrame(img: *Image)->bool throw
    {
        Debug.assert(me.initDone)
        if me.reachEnd:
            return false

        if !me.getFrame(true)
        {
            me.reachEnd = true
            return false
        }

        me.renderFrame(img.pixels.buffer, img.pf)
        return true
    }

    // Returns the number of frames
    // As Gif does not store it, we need to go threw all images (!)
    mtd getNumFrames()->s32 throw
    {
        Debug.assert(me.initDone)
        if me.numFrames:
            return me.numFrames

        me.numFrames = 1
        while me.getFrame(true):
            me.numFrames += 1

        me.rewind()
        return me.numFrames
    }

    // Restart at frame 0
    mtd rewind()
    {
        Debug.assert(me.initDone)
        me.reachEnd  = false
        me.stream    = me.streamStart + me.animStart
        me.streamLen = me.animRemain
    }
}

public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        let ext = Path.getExtensionLowerCase(fileName)
        return ext == ".gif"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        me.init(buffer, options.decodePixels)

        var result: retval
        me.getFrame(&result, options.decodePixels)
        if !options.decodePixels:
            return result

        me.renderFrame(result.pixels.buffer, result.pf)
        return result
    }
}
