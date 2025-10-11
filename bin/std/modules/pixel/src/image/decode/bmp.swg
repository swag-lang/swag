#global namespace Bmp
using Core, Core.Errors

const FILL_RLE_COMMAND     = 0
const FILL_RLE_ENDOFLINE   = 0
const FILL_RLE_ENDOFBITMAP = 1
const FILL_RLE_DELTA       = 2

public const BI_RGB = 0
public const BI_RLE8 = 1
public const BI_RLE4 = 2
public const BI_BITFIELDS = 3

#[Swag.Pack(1)]
public struct Header
{
    biSize:              u32
    biWidth:             s32
    biHeight:            s32
    biPlanes:            u16
    biBitCount:          u16
    biCompression:       u32
    biSizeImage:         u32
    biXPelsPerMeter:     u32
    biYPelsPerMeter:     u32
    biClrUsed:           u32
    biClrImportant:      u32
}

#[Swag.Pack(1)]
struct CoreHeader
{
    bcSize:         u32
    bcWidth:        u16
    bcHeight:       u16
    bcPlanes:       u16
    bcBitCount:     u16
}

#[Swag.Pack(1)]
struct FileHeader
{
    bfType:          u16
    bfSize:          u32
    bfReserved1:     u16
    bfReserved2:     u16
    bfOffBits:       u32
}

public struct Decoder {}

public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        return Path.getExtensionLowerCase(fileName) == ".bmp"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        var dataof  = @dataof(buffer)
        let countof = @countof(buffer)
        let last    = dataof + countof

        // FileHeader
        var bfOffBits: u32
        if countof >= #sizeof(FileHeader)
        {
            let fh = cast(const *FileHeader) dataof
            if fh.bfType == 0x4d42
            {
                dataof += #sizeof(FileHeader)
                bfOffBits = fh.bfOffBits
            }
        }

        // CoreHeader
        var bh:  Header
        let size = dref cast(const *u32) dataof
        if size == #sizeof(CoreHeader)
        {
            if dataof + #sizeof(CoreHeader) > last do
                throw InvalidFormatError{"invalid source buffer"}
            let ch = cast(const *CoreHeader) dataof
            dataof += #sizeof(Header)
            bh.biSize        = ch.bcSize
            bh.biWidth       = cast(s32) ch.bcWidth
            bh.biHeight      = cast(s32) ch.bcHeight
            bh.biPlanes      = ch.bcPlanes
            bh.biBitCount    = ch.bcBitCount
            bh.biCompression = BI_RGB
        }
        else
        {
            if dataof + #sizeof(Header) > last do
                throw InvalidFormatError{"invalid source buffer"}

            Memory.copy(&bh, dataof, #sizeof(Header))
            dataof += #sizeof(Header)
        }

        // Create image
        var result: retval

        switch bh.biBitCount
        {
        case 32:
            result.init(bh.biWidth, bh.biHeight, PixelFormat.BGRA8)
        case 24:
            result.init(bh.biWidth, bh.biHeight, PixelFormat.BGR8)
        default:
            throw UnsupportedFormatError{"unsupported bit count"}
        }

        // Palette
        if bh.biCompression != BI_RGB and bh.biCompression != BI_BITFIELDS do
            throw UnsupportedFormatError{"unsupported compression"}

        if !options.decodePixels do
            return result
        result.allocPixels()

        // Decode
        if bfOffBits do
            dataof = @dataof(buffer) + bfOffBits

        let bc8      = bh.biBitCount / 8
        let byteSize = cast(u64) (bh.biWidth * bh.biHeight * bc8)
        if dataof + byteSize > last do
            throw InvalidFormatError{"invalid source buffer"}

        switch bh.biBitCount
        {
        case 24:
            // Must be aligned to 4 bytes
            var c = (bh.biWidth * 3) & 3
            if c
            {
                c = 4 - c
                var dst  = result.pixels.buffer
                var src  = dataof
                let dstW = cast(u64) result.width * result.bpp8
                let srcW = dstW + c
                for result.height
                {
                    Memory.copy(dst, src, dstW)
                    dst += dstW
                    src += srcW
                }
            }
            else
            {
                Memory.copy(result.pixels.buffer, dataof, byteSize)
            }

        case 32:
            Memory.copy(result.pixels.buffer, dataof, byteSize)
        }

        return result
    }
}
