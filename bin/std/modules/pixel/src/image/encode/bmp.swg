#global namespace Bmp
using Core

public struct Encoder
{
}

public impl IImageEncoder for Encoder
{
    mtd canEncode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".bmp"
    }

    mtd encode(result: *ConcatBuffer, image: Image, options: EncodeOptions) throw
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
        psrc  := image.pixels

        if image.pf == .RGBA8
        {
            loop y: header.biHeight
            {
                loop x: header.biWidth
                {
                    result.addByte(psrc[2])
                    result.addByte(psrc[1])
                    result.addByte(psrc[0])
                    result.addByte(psrc[3])
                    psrc += 4
                }
            }
        }
        elif image.pf == .RGB8
        {
            loop y: header.biHeight
            {
                loop x: header.biWidth
                {
                    result.addByte(psrc[2])
                    result.addByte(psrc[1])
                    result.addByte(psrc[0])
                    psrc += 3
                }

                loop 4 - align
                    result.addByte(0)
            }
        }
        elif image.bpp != 24 or !align
        {
            result.addBytes(@mkslice(image.pixels, image.size))
        }
        else
        {
            loop y: header.biHeight
            {
                result.addBytes(@mkslice(psrc, header.biWidth * 3))
                psrc += header.biWidth * 3

                loop 4 - align
                    result.addByte(0)
            }
        }
    }
}