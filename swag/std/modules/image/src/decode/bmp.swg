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

impl Decoder
{
    public func decode(using self, buffer: const [..] u8)->Image
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

        // Check sizeImage to be sure its not greater than w*h
        if bh.biSizeImage > bh.biHeight * bh.biWidth * (bh.biBitCount >> 3)
            bh.biSizeImage = 0
        if bh.biSizeImage == 0
        {
            bh.biSizeImage = bh.biWidth * bh.biBitCount
            bh.biSizeImage = ((bh.biSizeImage + 31) & (~31)) / 8
            bh.biSizeImage *= bh.biHeight
        }

        // Create image
        var result: retval
        switch bh.biBitCount
        {
        case 32:
            result = create(bh.biWidth, bh.biHeight, PixelFormat.R8G8B8A8)
        case 24:
            result = create(bh.biWidth, bh.biHeight, PixelFormat.R8G8B8)
        default:
            throw "unsupported bit count"
        }

        // Palette
        if bh.biCompression != FILL_BI_RGB && bh.biCompression != FILL_BI_BITFIELDS
            throw "unsupported compression"

        // Decode
        if bfOffBits
            dataof = @dataof(buffer) + bfOffBits

        switch bh.biBitCount
        {
        case 32:
            if dataof + (bh.biWidth * bh.biHeight * 4) > last
                throw "invalid source buffer"

            pdest := result.pixels
            loop bh.biWidth * bh.biHeight
            {
                pdest[2] = dataof[0]
                pdest[1] = dataof[1]
                pdest[0] = dataof[2]
                pdest[3] = autocast (255 - dataof[3])
                pdest, dataof += 4
            }

        case 24:
            pdest := result.pixels

            // Alignement problem ? There's a padding at the end of each
            // line of 1 to 3 bytes
            c := (bh.biWidth * 3) & 3
            if c c = 4 - c

            if dataof + ((bh.biWidth + c) * bh.biHeight * 3) > last
                throw "invalid source buffer"

            // Encoded in B,G,R order, decode it in R,G,B order
            loop bh.biHeight
            {
                loop bh.biWidth
                {
                    pdest[2] = dataof[0]
                    pdest[1] = dataof[1]
                    pdest[0] = dataof[2]
                    pdest, dataof += 3
                }

                dataof += c
            }
        }

        return result
    }
}