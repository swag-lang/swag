#global namespace Tga
using Core, Core.Errors

public struct Encoder
{
}

public impl IImageEncoder for Encoder
{
    mtd impl canEncode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".tga"
    }

    mtd impl encode(result: *ConcatBuffer, image: Image, options: any) throw
    {
        if image.width > Swag.U16.Max  
            throw UnsupportedFormatError{message: "width is too big"}
        if image.height > Swag.U16.Max 
            throw UnsupportedFormatError{message: "height is too big"}

        var header: Header
        header.bpp           = image.bpp
        header.imageTypeCode = cast(u8) TYPE_RGB
        header.width         = cast(u16) image.width
        header.height        = cast(u16) image.height
        result.addStruct(header)
        result.addBytes(image.pixels.toSlice())
    }
}