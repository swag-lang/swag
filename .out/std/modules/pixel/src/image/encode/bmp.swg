#global namespace Bmp
using Core

public struct Encoder
{
}

public impl IEncoder for Encoder
{
    method canEncode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".bmp"
    }

    method encode(result: *ConcatBuffer, image: Image, options: EncodeOptions) throw
    {
        var fileHeader: FileHeader
        fileHeader.bfType    = 0x4d42
        fileHeader.bfSize    = cast(u32) image.size + @sizeof(FileHeader)
        fileHeader.bfOffBits = @sizeof(FileHeader) + @sizeof(Header)
        result.addStruct(fileHeader)

        var header: Header
        header.biSize        = @sizeof(Header)
        header.biWidth       = image.width
        header.biHeight      = image.height
        header.biPlanes      = 1
        header.biBitCount    = image.bpp
        header.biCompression = FILL_BI_RGB
        result.addStruct(header)

        align := (header.biWidth * 3) & 3
        if image.bpp != 24 or !align
            result.addBytes(@mkslice(image.pixels, image.size))
        else
        {
            psrc := image.pixels
            loop y: header.biHeight
            {
                result.addBytes(@mkslice(psrc, header.biWidth * 3))
                loop 4 - align
                    result.addByte(0)
                psrc += header.biWidth * 3
            }
        }
    }
}