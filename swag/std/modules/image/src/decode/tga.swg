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

public impl IDecoder for Decoder
{
    func canDecode(using self, fileName: string)->bool
    {
        return path.getLowerCaseExtension(fileName) == ".tga"
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
        case 32:
            result = create(cast(u32) header.width, cast(u32) header.height, PixelFormat.R8G8B8A8)
        case 24:
            result = create(cast(u32) header.width, cast(u32) header.height, PixelFormat.R8G8B8)
        default:
            throw "unsupported bit count"
        }

        dataof += header.size
        switch (header.imageTypeCode)
        {
        case TYPE_MAP, TYPE_RGB, TYPE_MONO:
            memory.copy(result.pixels, dataof, result.size)
        default:
            throw "unsupported format"
        }

        return result
    }
}