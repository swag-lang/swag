#global namespace Tga
using Core, Core.Errors

public struct Encoder {}

public impl IImageEncoder for Encoder
{
    mtd impl canEncode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".tga"
    }

    mtd impl encode(result: *ConcatBuffer, image: Image, options: any) throw
    {
        if image.width > Swag.U16.Max do
            throw UnsupportedFormatError{"width is too big"}
        if image.height > Swag.U16.Max do
            throw UnsupportedFormatError{"height is too big"}

        var header: Header
        header.bpp           = image.bpp
        header.imageTypeCode = cast(u8) TYPE_RGB
        header.width         = cast(u16) image.width
        header.height        = cast(u16) image.height
        result.addStruct(header)

        // Need to encode as BGR or BGRA
        switch image.pf
        {
        case RGB8:
            for var i = 0; i < image.pixels.count; i += 3
            {
                result.addNative(image.pixels.buffer[i + 2])
                result.addNative(image.pixels.buffer[i + 1])
                result.addNative(image.pixels.buffer[i + 0])
            }

        case RGBA8:
            for var i = 0; i < image.pixels.count; i += 4
            {
                result.addNative(image.pixels.buffer[i + 2])
                result.addNative(image.pixels.buffer[i + 1])
                result.addNative(image.pixels.buffer[i + 0])
                result.addNative(image.pixels.buffer[i + 3])
            }

        default:
            result.addBytes(image.pixels.toSlice())
        }
    }
}
