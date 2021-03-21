#global namespace tga
using core

const TYPE_MAP      = 1
const TYPE_RGB      = 2
const TYPE_MONO     = 3
const TYPE_RLE_MAP  = 9
const TYPE_RLE_RGB  = 10
const TYPE_RLE_MONO = 11

struct Header
{
    size:           u8
    colorMapType:   u8
    imageTypeCode:  u8
    originL:        u8
    originH:        u8
    paletteLengthL: u8
    paletteLengthH: u8
    bpcInPalette:   u8
    left:           u16
    top:            u16
    width:          u16
    height:         u16
    bpp:            u8
    descriptorByte: u8
}

public struct Decoder
{
}

impl Decoder
{
    func uncompress(bpp: u8, dstBuffer: *u8, srcBuffer: const *u8, lastSrc: const *u8, length: uint) throw
    {
        lastDest := dstBuffer + length
        dest := dstBuffer
        src := srcBuffer
        l := length
        bpp8 := bpp >> 3

        invalid := false
        while l > 0
        {
            if src >= lastSrc { invalid = true; break; }
            num := :src
            src += 1
            if num & 0x80
            {
                num -= 127
                val := :cast(const *u32) src
                src += bpp8
                if dest + bpp8 * num > lastDest { invalid = true; break; }

                loop num
                {
                    switch bpp
                    {
                    case 24:
                        dest[2] = bitcast(u8) (val >>% 16)
                        fallthrough
                    case 16:
                        dest[1] = bitcast(u8) (val >>% 8)
                        fallthrough
                    case 8:
                        dest[0] = bitcast(u8) val
                    default:
                        :cast(*u32) dest = val
                    }

                    dest += bpp8
                    l -= bpp8
                }
            }
            else
            {
                val := cast(uint) (bpp8 * (num + 1))
                if dest + val > lastDest { invalid = true; break; }
                if src + val > lastSrc { invalid = true; break; }
                memory.copy(dest, src, val)
                l -= val
                dest += val
                src += val
            }
        }

        if invalid
            throw "invalid source buffer"
    }
}

public impl IDecoder for Decoder
{
    func canDecode(using self, fileName: string)->bool
    {
        return path.getExtensionLowerCase(fileName) == ".tga"
    }

    func decode(using self, buffer: const [..] u8)->Image throw
    {
        dataof := @dataof(buffer)
        countof := @countof(buffer)
        last := dataof + countof

        if countof <= @sizeof(Header) throw "invalid source buffer"
        header := cast(const *Header) dataof
        dataof += @sizeof(Header)

        // Create image
        var result: retval
        switch header.bpp
        {
        case 24:
            result = create(cast(s32) header.width, cast(s32) header.height, PixelFormat.BGR8)
        case 32:
            result = create(cast(s32) header.width, cast(s32) header.height, PixelFormat.BGRA8)
        default:
            throw "unsupported bit count"
        }

        dataof += header.size
        switch (header.imageTypeCode)
        {
        case TYPE_MAP, TYPE_RGB, TYPE_MONO:
            if dataof + result.size > last throw "invalid source buffer"
            memory.copy(result.pixels, dataof, result.size)
        case TYPE_RLE_MAP, TYPE_RLE_RGB, TYPE_RLE_MONO:
            try uncompress(header.bpp, result.pixels, dataof, last, result.size)
        default:
            throw "unsupported type"
        }

        return result
    }
}