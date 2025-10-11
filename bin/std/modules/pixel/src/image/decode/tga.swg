#global namespace Tga
using Core, Core.Errors

const TYPE_MAP      = 1
const TYPE_RGB      = 2
const TYPE_MONO     = 3
const TYPE_RLE_MAP  = 9
const TYPE_RLE_RGB  = 10
const TYPE_RLE_MONO = 11

struct Header
{
    size:               u8
    colorMapType:       u8
    imageTypeCode:      u8
    originL:            u8
    originH:            u8
    paletteLengthL:     u8
    paletteLengthH:     u8
    bpcInPalette:       u8
    left:               u16
    top:                u16
    width:              u16
    height:             u16
    bpp:                u8
    descriptorByte:     u8
}

public struct Decoder {}

impl Decoder
{
    func uncompress(bpp: u8, dstBuffer: [*] u8, srcBuffer, lastSrc: const [*] u8, length: u64) throw
    {
        let lastDest = dstBuffer + length
        var dest     = dstBuffer
        var src      = srcBuffer
        var l        = length
        let bpp8     = cast(u32) (bpp >> 3)

        var invalid = false
        while l > 0
        {
            if src >= lastSrc
            {
                invalid = true
                break
            }
            var num = dref src
            src += 1
            if num & 0x80
            {
                num -= 127
                let val = dref cast(const *u32) src
                src += bpp8
                if dest + bpp8 * num > lastDest
                {
                    invalid = true
                    break
                }

                for num
                {
                    switch bpp
                    {
                    case 24:
                        dest[2] = cast #bit (u8) (val >> 16)
                        fallthrough
                    case 16:
                        dest[1] = cast #bit (u8) (val >> 8)
                        fallthrough
                    case 8:
                        dest[0] = cast #bit (u8) val
                    default:
                        dref cast(*u32) dest = val
                    }

                    dest += bpp8
                    l -= bpp8
                }
            }
            else
            {
                let val = cast(u64) bpp8 * (num + 1)
                if dest + val > lastDest
                {
                    invalid = true
                    break
                }
                if src + val > lastSrc
                {
                    invalid = true
                    break
                }
                Memory.copy(dest, src, val)
                l -= val
                dest += val
                src += val
            }
        }

        if invalid do
            throw InvalidFormatError{"invalid source buffer"}
    }
}

public impl IImageDecoder for Decoder
{
    func impl canDecode(me, fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".tga"
    }

    func impl decode(me, buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        var dataof  = @dataof(buffer)
        let countof = @countof(buffer)
        let last    = dataof + countof

        if countof <= #sizeof(Header) do
            throw InvalidFormatError{"invalid source buffer"}
        let header = cast(const *Header) dataof
        dataof += #sizeof(Header)

        // Create image
        var result: retval
        switch header.bpp
        {
        case 24:
            result.init(header.width, header.height, PixelFormat.BGR8)
        case 32:
            result.init(header.width, header.height, PixelFormat.BGRA8)
        default:
            throw UnsupportedFormatError{"unsupported bit depth"}
        }

        if !options.decodePixels do
            return result
        result.allocPixels()

        dataof += header.size
        switch header.imageTypeCode
        {
        case TYPE_MAP, TYPE_RGB, TYPE_MONO:
            if dataof + result.size > last do
                throw InvalidFormatError{"invalid source buffer"}
            Memory.copy(result.pixels.buffer, dataof, result.size)

        case TYPE_RLE_MAP, TYPE_RLE_RGB, TYPE_RLE_MONO:
            uncompress(header.bpp, result.pixels.buffer, dataof, last, result.size)

        default:
            throw UnsupportedFormatError{"unsupported type"}
        }

        return result
    }
}
