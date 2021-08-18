#global namespace Tga
using Core

public struct Encoder
{
}

public impl IEncoder for Encoder
{
    method canEncode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".tga"
    }

    method encode(result: *ConcatBuffer, image: Image, options: EncodeOptions) throw
    {
        if image.width > Swag.U16.Max  throw "image width is too big"
        if image.height > Swag.U16.Max throw "image height is too big"

        var header: Header
        header.bpp           = image.bpp
        header.imageTypeCode = cast(u8) TYPE_RGB
        header.width         = cast(u16) image.width
        header.height        = cast(u16) image.height
        result.addStruct(header)
        result.addBytes(@mkslice(image.pixels, image.size))
    }
}