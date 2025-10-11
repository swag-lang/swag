#global namespace Bmp
using Core

public struct Encoder {}

public impl IImageEncoder for Encoder
{
    mtd impl canEncode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".bmp"
    }

    mtd impl encode(result: *ConcatBuffer, image: Image, options: any) throw
    {
        var fileHeader: FileHeader
        fileHeader.bfType    = 0x4d42
        fileHeader.bfSize    = cast(u32) image.size + #sizeof(FileHeader)
        fileHeader.bfOffBits = #sizeof(FileHeader) + #sizeof(Header)
        result.addStruct(fileHeader)

        var header: Header
        header.biSize        = #sizeof(Header)
        header.biWidth       = image.width
        header.biHeight      = image.height
        header.biPlanes      = 1
        header.biBitCount    = image.bpp
        header.biCompression = BI_RGB
        result.addStruct(header)

        let align = (header.biWidth * 3) & 3
        var psrc  = image.pixels.buffer

        if image.pf == .RGBA8
        {
            for y in header.biHeight
            {
                for x in header.biWidth
                {
                    result.addNative(psrc[2])
                    result.addNative(psrc[1])
                    result.addNative(psrc[0])
                    result.addNative(psrc[3])
                    psrc += 4
                }
            }
        }
        elif image.pf == .RGB8
        {
            for y in header.biHeight
            {
                for x in header.biWidth
                {
                    result.addNative(psrc[2])
                    result.addNative(psrc[1])
                    result.addNative(psrc[0])
                    psrc += 3
                }

                if align
                {
                    for 4 - align do
                        result.addNative(0'u8)
                }
            }
        }
        elif image.bpp != 24 or !align
        {
            result.addBytes(image.pixels.toSlice())
        }
        else
        {
            for y in header.biHeight
            {
                result.addBytes(@mkslice(psrc, header.biWidth * 3))
                psrc += header.biWidth * 3

                for 4 - align do
                    result.addNative(0'u8)
            }
        }
    }
}
