#global namespace bmp
using core

const FILL_RLE_COMMAND     = 0
const FILL_RLE_ENDOFLINE   = 0
const FILL_RLE_ENDOFBITMAP = 1
const FILL_RLE_DELTA       = 2
const FILL_BI_RGB          = 0
const FILL_BI_RLE8         = 1
const FILL_BI_RLE4         = 2
const FILL_BI_BITFIELDS    = 3

#[swag.pack(1)]
struct Header
{
    biSize:             u32
    biWidth:            u32
    biHeight:           u32
    biPlanes:           u16
    biBitCount:         u16
    biCompression:      u32
    biSizeImage:        u32
    biXPelsPerMeter:    u32
    biYPelsPerMeter:    u32
    biClrUsed:          u32
    biClrImportant:     u32
}

#[swag.pack(1)]
struct CoreHeader
{
    bcSize:     u32
    bcWidth:    u16
    bcHeight:   u16
    bcPlanes:   u16
    bcBitCount: u16
}

#[swag.pack(1)]
struct FileHeader
{
    bfType:         u16
    bfSize:         u32
    bfReserved1:    u16
    bfReserved2:    u16
    bfOffBits:      u32
}

public struct Decoder
{
}

public impl IDecoder for Decoder
{
    func canDecode(using self, fileName: string)->bool
    {
        return path.getExtensionLowerCase(fileName) == ".bmp"
    }

    func decode(using self, buffer: const [..] u8)->Image throw
    {
        dataof := @dataof(buffer)
        countof := @countof(buffer)
        last := dataof + countof

        // FileHeader
        var bfOffBits: u32
        if countof >= @sizeof(FileHeader)
        {
            fh := cast(const *FileHeader) dataof
            if fh.bfType == 0x4d42
            {
                dataof += @sizeof(FileHeader)
                bfOffBits = fh.bfOffBits
            }
        }

        // CoreHeader
        var bh: Header
        size := :cast(const *u32) dataof
        if size == @sizeof(CoreHeader)
        {
            if dataof + @sizeof(CoreHeader) > last
                throw "invalid source buffer"
            ch := cast(const *CoreHeader) dataof
            dataof += @sizeof(Header)
            bh.biSize        = ch.bcSize
            bh.biWidth       = cast(u32) ch.bcWidth
            bh.biHeight      = cast(u32) ch.bcHeight
            bh.biPlanes      = ch.bcPlanes
            bh.biBitCount    = ch.bcBitCount
            bh.biCompression = FILL_BI_RGB
        }
        else
        {
            if dataof + @sizeof(Header) > last
                throw "invalid source buffer"
            memory.copy(&bh, dataof, @sizeof(Header))
            dataof += @sizeof(Header)
        }

        // Create image
        var result: retval
        switch bh.biBitCount
        {
        case 32:
            result = create(bh.biWidth, bh.biHeight, PixelFormat.BGRA8)
        case 24:
            result = create(bh.biWidth, bh.biHeight, PixelFormat.BGR8)
        default:
            throw "unsupported bit count"
        }

        // Palette
        if bh.biCompression != FILL_BI_RGB && bh.biCompression != FILL_BI_BITFIELDS
            throw "unsupported compression"

        // Decode
        if bfOffBits
            dataof = @dataof(buffer) + bfOffBits

        byteSize := cast(uint) (bh.biWidth * bh.biHeight * (bh.biBitCount / 8))
        if dataof + byteSize > last
            throw "invalid source buffer"

        switch bh.biBitCount
        {
        case 24, 32:
            memory.copy(result.pixels, dataof, byteSize)
        }

        return result
    }
}