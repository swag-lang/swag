#global namespace bmp

const FILL_RLE_COMMAND     = 0
const FILL_RLE_ENDOFLINE   = 0
const FILL_RLE_ENDOFBITMAP = 1
const FILL_RLE_DELTA       = 2
const FILL_BI_RGB          = 0
const FILL_BI_RLE8         = 1
const FILL_BI_RLE4         = 2
const FILL_BI_BITFIELDS    = 3

struct Header
{
    biSize:             u32
    biWidth:            s32
    biHeight:           s32
    biPlanes:           u16
    biBitCount:         u16
    biCompression:      u32
    biSizeImage:        u32
    biXPelsPerMeter:    s32
    biYPelsPerMeter:    s32
    biClrUsed:          u32
    biClrImportant:     u32
}

struct CoreHeader
{
    bcSize:     u32
    bcWidth:    u16
    bcHeight:   u16
    bcPlanes:   u16
    bcBitCount: u16
}

struct FileHeader
{
    bfType:         u16
    bfSize:         u32
    bfReserved1:    u16
    bfReserved2:    u16
    bfOffBits:      u32
}