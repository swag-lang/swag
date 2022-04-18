// :LICENCE
// This code is based on "gifdec" (https://github.com/lecram/gifdec)
// The original code has been modified for Swag

#global namespace Gif
using Swag, Core

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

#[Swag.Opaque]
public struct Decoder
{
    fd:         s32
    width:      u16
    height:     u16
    depth:      u16
    loopCount:  u16
    gce:        Gce
    palette:    *Palette
    lct:        Palette
    gct:        Palette
    fx:         u16
    fy:         u16
    fw:         u16
    fh:         u16
    bgIndex:    u8
    canvas:     *u8
    frame:      Array'u8
    table:      Table

    streamStart:    const *u8
    stream:         const *u8
    streamLen:      uint
}

public impl Decoder
{
    mtd seek(len: uint) throw
    {
        if len > streamLen
            throw "read error"
        stream    += len
        streamLen -= len
    }

    mtd read(dest: *u8, len: uint) throw
    {
        if len > streamLen
            throw "read error"
        Memory.copy(dest, stream, len)
        stream    += len
        streamLen -= len
    }

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

    mtd init(buffer: const [..] u8, options: DecodeOptions) throw
    {
        streamStart, stream = @dataof(buffer)
        streamLen = @countof(buffer)

        /* Header */
        var sigver: [3] u8
        try read(sigver, 3)
        if Memory.compare(sigver, @dataof("GIF"), 3) != 0
            throw "invalid signature"

        /* Version */
        try read(sigver, 3)
        if Memory.compare(sigver, @dataof("89a"), 3) != 0
            throw "invalid version"

        /* Width x Height */
        width  = try readU16()
        height = try readU16()

        /* FDSZ */
        var fdsz: u8
        try read(&fdsz, 1)
        if !(fdsz & 0x80) // Presence of GCT
            throw "no global color table"

        /* Color space's depth */
        depth = acast (((fdsz >>,safe 4) & 7) + 1)
        /* Ignore sort flag */
        /* GCT Size */
        gct.size = 1 << ((fdsz & 0x07) + 1)
        /* Background color index */
        try read(&bgIndex, 1)
        /* Aspect ratio */
        var aspect: u8
        try read(&aspect, 1)

        try read(gct.colors, 3'uint * gct.size)
        palette = &gct

        if options.decodePixels
        {
            frame.resize(4'uint * width * height)
            Memory.set(frame.buffer, bgIndex, cast(uint) (width * height))
            canvas  = &frame.buffer[width * height]

            bgcolor := &palette.colors[bgIndex*3]
            if bgcolor[0] or bgcolor[1] or bgcolor [2]
            {
                loop i: width * height
                    Memory.copy(&canvas[i*3], bgcolor, 3)
            }
        }
    }

    mtd discardSubBlocks() throw
    {
        loop
        {
            size := try readU8()
            if !size
                break
            try seek(acast size)
        }
    }

    mtd readPlainTextExt() throw
    {
        /* Discard plain text metadata. */
        try seek(13)
        /* Discard plain text sub-blocks. */
        try discardSubBlocks()
    }

    mtd readGraphicControlExt() throw
    {
        /* Discard block size (always 0x04) */
        try seek(1)
        rdit := try readU8()
        gce.disposal = (rdit >>,safe 2) & 3
        gce.input = rdit & 2
        gce.transparency = rdit & 1
        gce.delay = try readU16()
        gce.tindex = try readU8()
        /* Skip block terminator */
        try seek(1)
    }

    mtd readCommentExt() throw
    {
        try discardSubBlocks()
    }

    mtd readApplicationExt() throw
    {
        var app_id: [8] u8
        var app_auth_code: [3] u8

        /* Discard block size (always 0x0B) */
        discard try readU8()
        /* Application Identifier */
        try read(app_id, 8);
        /* Application Authentication Code */
        try read(app_auth_code, 3)

        if !Memory.compare(app_id, @dataof("NETSCAPE"), @countof(app_id))
        {
            /* Discard block size (0x03) and constant byte (0x01). */
            try seek(2)
            loopCount = try readU16()
            /* Skip block terminator. */
            try seek(1)
        }
        else
        {
            try discardSubBlocks()
        }
    }

    mtd readExt() throw
    {
        label := try readU8()
        switch label
        {
        case 0x01:
            try readPlainTextExt()
        case 0xF9:
            try readGraphicControlExt()
        case 0xFE:
            try readCommentExt()
        case 0xFF:
            try readApplicationExt()
        default:
            throw Format.toString("unknown extension: %", label)
        }
    }

    mtd initTable(key_size: s32)
    {
        table.bulk = Math.max(1 << (key_size + 1), 0x100)
        table.nentries = (1 << key_size) + 2
        table.entries.resize(acast table.bulk)
        loop key: 1 << key_size
            table.entries[key] = @{1, 0xFFF, cast(u8) key}
    }

    mtd getKey(key_size: s32, sub_len, shift, byte: *u8)->u16 throw
    {
        key := 0'u16
        frag_size := 0
        for bits_read := 0; bits_read < key_size; bits_read += frag_size
        {
            rpad := (dref shift + bits_read) % 8
            if rpad == 0
            {
                /* Update byte. */
                if dref sub_len == 0
                {
                    dref sub_len = try readU8() /* Must be nonzero! */
                    if dref sub_len == 0
                        return 0x1000
                }

                dref byte = try readU8()
                sub_len[0] -= 1
            }

            frag_size = Math.min(cast(s32) (key_size - bits_read), cast(s32) (8 - rpad))
            key |= (cast(u16) ((dref byte) >>,safe rpad)) <<,safe bits_read
        }

        /* Clear extra bits to the left. */
        key &= cast(u16) ((1 << key_size) - 1)
        dref shift = cast(u8) ((dref shift + key_size) % 8)
        return key
    }

    mtd addEntry(length: u16, prefix: u16, suffix: u8)->s32 throw
    {
        if table.nentries == table.bulk
        {
            table.bulk *= 2
            table.entries.resize(cast(uint) table.bulk)
        }

        table.entries[table.nentries] = @{length, prefix, suffix}
        table.nentries += 1
        if (table.nentries & (table.nentries - 1)) == 0
            return 1
        return 0
    }

    mtd readImageData(interlace: u8) throw
    {
        byte := try readU8()
        key_size := cast(s32) byte
        if key_size < 2 or key_size > 8
            throw "invalid frame"

        start := cast(uint) (stream - streamStart)
        remainStart := streamLen
        try discardSubBlocks()
        end := cast(uint) (stream - streamStart)
        remainEnd := streamLen
        stream = streamStart + start
        streamLen = remainStart

        clear := 1 << key_size
        stop  := clear + 1
        initTable(key_size)
        key_size += 1
        init_key_size := key_size
        sub_len, shift := 0'u8
        key := try getKey(key_size, &sub_len, &shift, &byte) /* clear code */
        frm_off := 0
        ret := 0
        frm_size := fw*fh
        table_is_full := 0
        str_len := 0'u16

        var entry: Entry
        while frm_off < frm_size
        {
            if key == clear
            {
                key_size = init_key_size
                table.nentries = (1 << (key_size - 1)) + 2
                table_is_full = 0
            }
            elif !table_is_full
            {
                ret = try addEntry(cast(u16) (str_len + 1), key, entry.suffix)
                if table.nentries == 0x1000
                {
                    ret = 0
                    table_is_full = 1
                }
            }

            key = try getKey(key_size, &sub_len, &shift, &byte)
            if key == clear continue
            if key == stop or key == 0x1000 break
            if ret == 1 key_size += 1
            entry = table.entries.buffer[key]
            str_len = entry.length
            loop i: str_len
            {
                p := frm_off + entry.length - 1
                x := p % fw
                y := p / fw
                //if (interlace)
                //    y = interlaced_line_index((int) gif->fh, y);
                frame[(fy + y) * width + fx + x] = entry.suffix
                if entry.prefix == 0xFFF
                    break
                entry = table.entries.buffer[entry.prefix]
            }

            frm_off += str_len
            if key < table.nentries - 1 and !table_is_full
                table.entries[table.nentries - 1].suffix = entry.suffix
        }

        if key == stop
            sub_len = try readU8() /* Must be zero! */
        stream = streamStart + end
        streamLen = remainEnd
    }

    mtd readImage() throw
    {
        /* Image descriptor */
        fx = try readU16()
        fy = try readU16()
        if fx >= width or fy >= height
            throw "invalid frame size"

        fw = try readU16()
        fh = try readU16()
        fw = Math.min(fw, cast(u16) (width - fx))
        fh = Math.min(fh, cast(u16) (height - fy))

        fisrz := try readU8()
        interlace := fisrz & 0x40

        /* Ignore Sort Flag. */
        /* Local Color Table? */
        if fisrz & 0x80
        {
            /* Read LCT */
            lct.size = 1 << ((fisrz & 0x07) + 1)
            try read(lct.colors, 3'uint * lct.size)
            palette = &lct
        }
        else
            palette = &gct

        try readImageData(interlace)
    }

    mtd renderFrame(buffer: *u8, pf: PixelFormat)
    {
        i := fy * width + fx

        if pf.hasAlpha()
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
        sep := try readU8()
        while sep != ","'u8
        {
            if sep == ";"'u8
                return 0
            if sep != "!"'u8
                throw "invalid format"
            try readExt()
            sep = try readU8()
        }

        if decodePixels
            try readImage()
        return 1
    }
}

public impl IImageDecoder for Decoder
{
    mtd canDecode(fileName: string)->bool
    {
        ext := Path.getExtensionLowerCase(fileName)
        return ext == ".gif"
    }

    mtd decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        try init(buffer, options)
        discard try getFrame(options.decodePixels)

        var result: retval
        result.init(acast width, acast height, gce.transparency ? PixelFormat.RGBA8 : PixelFormat.RGB8)
        if !options.decodePixels
            return result
        result.allocPixels()

        renderFrame(result.pixels, result.pf)
        return result
    }
}