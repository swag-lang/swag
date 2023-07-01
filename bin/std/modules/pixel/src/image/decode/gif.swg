// :LICENCE
// This code is based on "gifdec" (https://github.com/lecram/gifdec)
// The original code has been modified for Swag

#global namespace Gif
using Swag, Core

#scopefile {

struct Entry
{
    length: u16
    prefix: u16
    suffix: u8
}

struct Table
{
    bulk:       s32
    nentries:   s32
    entries:    Array'Entry
}

struct Palette
{
    size:   s32
    colors: [0x100 * 3] u8
}

struct Gce
{
    delay:          u16
    tindex:         u8
    disposal:       u8
    input:          s32
    transparency:   s32
}
}

#[Swag.Opaque]
public struct Decoder
{
    fd:         s32
    width:      u16
    height:     u16
    depth:      u16
    loopCount:  u16
    numFrames:  s32
    gce:        Gce
    palette:    *Palette
    lct:        Palette
    gct:        Palette
    fx:         u16
    fy:         u16
    fw:         u16
    fh:         u16
    bgIndex:    u8
    canvas:     ^u8
    frame:      Array'u8
    table:      Table
    animStart:  u64
    animRemain: u64
    reachEnd:   bool
    initDone:   bool

    streamStart:    const ^u8
    stream:         const ^u8
    streamLen:      u64
}

impl Decoder
{
    mtd seek(len: u64) throw
    {
        if len > streamLen
            throw "read error"
        stream    += len
        streamLen -= len
    }

    mtd read(dest: *u8, len: u64) throw
    {
        if len > streamLen
            throw "read error"
        Memory.copy(dest, stream, len)
        stream    += len
        streamLen -= len
    }

    #[Swag.Inline]
    mtd readU8()->u8 throw
    {
        if streamLen < 1
            throw "read error"
        res := stream[0]
        stream    += 1
        streamLen -= 1
        return res
    }

    mtd readU16()->u16 throw
    {
        if streamLen < 2
            throw "read error"
        res := cast(u16) (stream[0] + (cast(u16) stream[1] << 8))
        stream += 2
        streamLen -= 2
        return res
    }

    mtd discardSubBlocks() throw
    {
        loop
        {
            size := readU8()
            if !size
                break
            seek(acast size)
        }
    }

    mtd readPlainTextExt() throw
    {
        /* Discard plain text metadata. */
        seek(13)
        /* Discard plain text sub-blocks. */
        discardSubBlocks()
    }

    mtd readGraphicControlExt() throw
    {
        /* Discard block size (always 0x04) */
        seek(1)
        rdit := readU8()
        gce.disposal = (rdit >> 2) & 3
        gce.input = rdit & 2
        gce.transparency = rdit & 1
        gce.delay = readU16()
        gce.tindex = readU8()
        /* Skip block terminator */
        seek(1)
    }

    mtd readCommentExt() throw
    {
        discardSubBlocks()
    }

    mtd readApplicationExt() throw
    {
        var app_id: [8] u8
        var app_auth_code: [3] u8

        /* Discard block size (always 0x0B) */
        seek(1)
        /* Application Identifier */
        read(app_id, 8);
        /* Application Authentication Code */
        read(app_auth_code, 3)

        if !Memory.compare(app_id, @dataof("NETSCAPE"), @countof(app_id))
        {
            /* Discard block size (0x03) and constant byte (0x01). */
            seek(2)
            loopCount = readU16()
            /* Skip block terminator. */
            seek(1)
        }
        else
        {
            discardSubBlocks()
        }
    }

    mtd readExt() throw
    {
        label := readU8()
        switch label
        {
        case 0x01:
            readPlainTextExt()
        case 0xF9:
            readGraphicControlExt()
        case 0xFE:
            readCommentExt()
        case 0xFF:
            readApplicationExt()
        default:
            throw Format.toString("unknown extension: %", label)
        }
    }

    mtd initTable(keySize: s32)
    {
        table.bulk = Math.max(1 << (keySize + 1), 0x100)
        table.nentries = (1 << keySize) + 2
        table.entries.resize(acast table.bulk)
        loop key: 1 << keySize
            table.entries[key] = {1, 0xFFF, cast(u8) key}
    }

    mtd getKey(keySize: s32, subLen, shift, byte: *u8)->u16 throw
    {
        key := 0'u16
        frag_size := 0
        for bits_read := 0; bits_read < keySize; bits_read += frag_size
        {
            rpad := (dref shift + bits_read) % 8
            if rpad == 0
            {
                /* Update byte. */
                if dref subLen == 0
                {
                    dref subLen = readU8() /* Must be nonzero! */
                    if dref subLen == 0
                        return 0x1000
                }

                dref byte = readU8()
                dref subLen -= 1
            }

            frag_size = Math.min(cast(s32) (keySize - bits_read), cast(s32) (8 - rpad))
            key |= (cast(u16) ((dref byte) >> rpad)) << bits_read
        }

        /* Clear extra bits to the left. */
        key &= cast(u16) ((1 << keySize) - 1)
        dref shift = cast(u8) ((dref shift + keySize) % 8)
        return key
    }

    mtd addEntry(length: u16, prefix: u16, suffix: u8)->s32
    {
        if table.nentries == table.bulk
        {
            table.bulk *= 2
            table.entries.resize(cast(u64) table.bulk)
        }

        table.entries[table.nentries] = {length, prefix, suffix}
        table.nentries += 1
        if (table.nentries & (table.nentries - 1)) == 0
            return 1
        return 0
    }

    func interlacedLineIndex(h0, y0: s32)->s32
    {
        h := h0
        y := y0
        p := (h - 1) / 8 + 1
        if (y < p) /* pass 1 */
            return y * 8
        y -= p
        p = (h - 5) / 8 + 1
        if (y < p) /* pass 2 */
            return y * 8 + 4
        y -= p
        p = (h - 3) / 4 + 1
        if (y < p) /* pass 3 */
            return y * 4 + 2
        y -= p
        /* pass 4 */
        return y * 2 + 1
    }

    mtd readImageData(interlace: u8) throw
    {
        byte := readU8()
        keySize := cast(s32) byte
        if keySize < 2 or keySize > 8
            throw "invalid frame"

        start := cast(u64) (stream - streamStart)
        remainStart := streamLen
        discardSubBlocks()
        end := cast(u64) (stream - streamStart)
        remainEnd := streamLen
        stream = streamStart + start
        streamLen = remainStart

        clear := 1 << keySize
        stop  := clear + 1
        initTable(keySize)
        keySize += 1
        initKeySize := keySize
        subLen, shift := 0'u8
        key := getKey(keySize, &subLen, &shift, &byte) /* clear code */
        frmOff := 0
        ret := 0
        frmSize := cast(u32) fw * fh
        tableIsFull := 0
        strLen := 0'u16

        var entry: Entry
        while frmOff < frmSize
        {
            if key == clear
            {
                keySize = initKeySize
                table.nentries = (1 << (keySize - 1)) + 2
                tableIsFull = 0
            }
            elif !tableIsFull
            {
                ret = addEntry(cast(u16) (strLen + 1), key, entry.suffix)
                if table.nentries == 0x1000
                {
                    ret = 0
                    tableIsFull = 1
                }
            }

            key = getKey(keySize, &subLen, &shift, &byte)
            if key == clear continue
            if key == stop or key == 0x1000 break
            if ret == 1 keySize += 1
            entry = table.entries.buffer[key]
            strLen = entry.length
            loop i: strLen
            {
                p := frmOff + entry.length - 1
                x := p % fw
                y := p / fw
                if interlace
                    y = interlacedLineIndex(cast(s32) fh, y)
                frame[(fy + y) * width + fx + x] = entry.suffix
                if entry.prefix == 0xFFF
                    break
                entry = table.entries.buffer[entry.prefix]
            }

            frmOff += strLen
            if key < table.nentries - 1 and !tableIsFull
                table.entries[table.nentries - 1].suffix = entry.suffix
        }

        if key == stop
            subLen = readU8() /* Must be zero! */
        stream = streamStart + end
        streamLen = remainEnd
    }

    mtd readImage() throw
    {
        /* Image descriptor */
        fx = readU16()
        fy = readU16()
        if fx >= width or fy >= height
            throw "invalid frame size"

        fw = readU16()
        fh = readU16()
        fw = Math.min(fw, cast(u16) (width - fx))
        fh = Math.min(fh, cast(u16) (height - fy))

        fisrz := readU8()
        interlace := fisrz & 0x40

        /* Ignore Sort Flag. */
        /* Local Color Table? */
        if fisrz & 0x80
        {
            /* Read LCT */
            lct.size = 1 << ((fisrz & 0x07) + 1)
            read(lct.colors, 3'u64 * lct.size)
            palette = &lct
        }
        else
            palette = &gct

        readImageData(interlace)
    }

    mtd renderFrame(buffer: ^u8, pf: PixelFormat)
    {
        i := cast(u32) fy * width + fx

        if pf.hasAlpha()
        {
            loop j: fh-1..0
            {
                loop k: cast(u32) fw
                {
                    index := frame[(fy + j) * cast(u32) width + (fx + k)]
                    color := &palette.colors[index*3'u32]
                    b     := buffer + (i+k)*4
                    b[0] = color[0]
                    b[1] = color[1]
                    b[2] = color[2]
                    b[3] = index == gce.tindex ? 0'u8 : 0xFF'u8
                }

                i += width
            }
        }
        else
        {
            loop j: fh-1..0
            {
                loop k: fw
                {
                    index := frame[(fy + j) * width + (fx + k)]
                    color := &palette.colors[index*3]
                    b     := buffer + (i+k)*4
                    b[0] = color[0]
                    b[1] = color[1]
                    b[2] = color[2]
                }

                i += width
            }
        }
    }

    mtd getFrame(decodePixels: bool)->s32 throw
    {
        sep := readU8()
        while sep != ","'u8
        {
            if sep == ";"'u8
                return 0
            if sep != "!"'u8
                throw "invalid format"
            readExt()
            sep = readU8()
        }

        if decodePixels
            readImage()
        return 1
    }
}

public impl Decoder
{
    // Initialize the decoder
    mtd init(buffer: const [..] u8, decodePixels: bool) throw
    {
        initDone = true
        streamStart, stream = @dataof(buffer)
        streamLen = @countof(buffer)

        /* Header */
        var sigver: [3] u8
        read(sigver, 3)
        if Memory.compare(sigver, @dataof("GIF"), 3) != 0
            throw "invalid signature"

        /* Version */
        read(sigver, 3)
        if Memory.compare(sigver, @dataof("89a"), 3) != 0
            throw "invalid version"

        /* Width x Height */
        width  = readU16()
        height = readU16()

        /* FDSZ */
        var fdsz: u8
        read(&fdsz, 1)
        if !(fdsz & 0x80) // Presence of GCT
            throw "no global color table"

        /* Color space's depth */
        depth = acast (((fdsz >> 4) & 7) + 1)
        /* Ignore sort flag */
        /* GCT Size */
        gct.size = 1 << ((fdsz & 0x07) + 1)
        /* Background color index */
        bgIndex = readU8()
        /* Aspect ratio */
        discard readU8()

        read(gct.colors, 3'u64 * gct.size)
        palette = &gct

        if decodePixels
        {
            size := cast(u64) width * cast(u64) height
            frame.resize(4 * size)
            Memory.set(frame.buffer, bgIndex, size)
            canvas  = &frame.buffer[size]

            bgcolor := &palette.colors[bgIndex*3'u32]
            if bgcolor[0] or bgcolor[1] or bgcolor[2]
            {
                loop i: size
                    Memory.copy(&canvas[i*3], bgcolor, 3)
            }
        }

        animStart = cast(u64) (stream - streamStart)
        animRemain = streamLen

        discard getFrame(decodePixels)
    }

    // Create an image for the current frame
    mtd getFrame(image: *Image, decodePixels = true)
    {
        Debug.assert(initDone)
        image.init(acast width, acast height, gce.transparency ? PixelFormat.RGBA8 : PixelFormat.RGB8)
        if !decodePixels
            return
        image.allocPixels()
    }

    // Get the next frame. Returns false if we have reached the end
    mtd nextFrame(img: *Image)->bool throw
    {
        Debug.assert(initDone)
        if reachEnd
            return false

        if !getFrame(true)
        {
            reachEnd = true
            return false
        }

        renderFrame(img.pixels.buffer, img.pf)
        return true
    }

    // Returns the number of frames
    // As Gif does not store it, we need to go threw all images (!)
    mtd getNumFrames()->s32 throw
    {
        Debug.assert(initDone)
        if numFrames
            return numFrames

        numFrames = 1
        while getFrame(true)
            numFrames += 1

        rewind()
        return numFrames
    }

    // Restart at frame 0
    mtd rewind()
    {
        Debug.assert(initDone)
        reachEnd = false
        stream = streamStart + animStart
        streamLen = animRemain
    }
}

public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        ext := Path.getExtensionLowerCase(fileName)
        return ext == ".gif"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        init(buffer, options.decodePixels)

        var result: retval
        getFrame(&result, options.decodePixels)
        if !options.decodePixels
            return result

        renderFrame(result.pixels.buffer, result.pf)
        return result
    }
}