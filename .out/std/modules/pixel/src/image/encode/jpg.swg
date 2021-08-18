#global namespace Jpg
using Core

struct APP0infotype
{
    marker: u16
    length: u16
    JFIFsignature: [5] u8
    versionhi: u8
    versionlo: u8
    xyunits: u8
    xdensity: u16
    ydensity: u16
    thumbnwidth: u8
    thumbnheight: u8
}

struct SOF0infotype
{
    marker: u16
    length: u16
    precision: u8
    height: u16
    width: u16
    nrofcomponents: u8
    IdY: u8
    HVY: u8
    QTY: u8
    IdCb: u8
    HVCb: u8
    QTCb: u8
    IdCr: u8
    HVCr: u8
    QTCr: u8
}

struct SOSinfotype
{
    marker: u16
    length: u16
    nrofcomponents: u8
    IdY: u8
    HTY: u8
    IdCb: u8
    HTCb: u8
    IdCr: u8
    HTCr: u8
    Ss: u8
    Se: u8
    Bf: u8
}

struct ColorRGB
{
    b, g, r: u8
}

const APP0info = APP0infotype{0xFFE0, 16, @["J"'u8, "F"'u8, "I"'u8, "F"'u8, 0], 1, 1, 0, 1, 1, 0, 0}
const SOF0info = SOF0infotype{0xFFC0, 17, 8, 0, 0, 3, 1, 0x11, 0, 2, 0x11, 1, 3, 0x11, 1}
const SOSinfo  = SOSinfotype{0xFFDA, 12, 3, 1, 0, 2, 0x11, 3, 0x11, 0, 0x3F, 0}

const std_dc_luminance_nrcodes   = @[0'u8, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0]
const std_dc_luminance_values    = @[0'u8, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
const std_dc_chrominance_nrcodes = @[0'u8, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0]
const std_dc_chrominance_values  = @[0'u8, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
const std_ac_luminance_nrcodes   = @[0'u8, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d]
const std_ac_chrominance_nrcodes = @[0'u8, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77]
const mask                       = @[1'u16, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768]

const zigzag: [] u8 = @[
    0,  1,  5,  6,  14, 15, 27, 28, 2,  4,  7,  13, 16, 26, 29, 42, 3,  8,  12, 17, 25, 30,
    41, 43, 9,  11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38,
    46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63]

const std_luminance_qt: [] u8 = @[
    16, 11, 10, 16, 24, 40,  51,  61, 12, 12, 14, 19, 26, 58,  60,  55, 14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99]

const std_chrominance_qt: [] u8 = @[
    17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99]

const std_ac_luminance_values: [] u8 = @[
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa]

const std_ac_chrominance_values: [] u8 = @[
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa]

#[Swag.Opaque]
public struct Encoder
{
    imageX:     u16
    imageY:     u16
    bufferRGB:  *ColorRGB
}

impl Encoder
{
    method loadBitmap(image: Image)
    {
        imageX = cast(u16) image.width
        imageY = cast(u16) image.height

        var xDiv8, yDiv8: u16
        if imageX % 8 != 0
            xDiv8 = cast(u16) ((imageX / 8) * 8 + 8)
        else
            xDiv8 = imageX
        if imageY % 8 != 0
            yDiv8 = cast(u16) ((imageY / 8) * 8 + 8)
        else
            yDiv8 = imageY

        bufferRGB = Memory.new'ColorRGB(cast(uint) (xDiv8 * yDiv8))
        pref := image.pixels
        loop nrline: imageY
        {
            switch image.bpp
            {
            case 24:
                Memory.copy(bufferRGB + nrline * xDiv8, pref, cast(uint) (imageX * 3))
                pref += imageX * 3

            case 32:
                loop column: imageX
                {
                    bufferRGB[column + nrline * xDiv8].b = pref[0]
                    bufferRGB[column + nrline * xDiv8].g = pref[1]
                    bufferRGB[column + nrline * xDiv8].r = pref[2]
                    pref += 4;
                }
            }

            var lastcolor: ColorRGB = ?
            Memory.copy(&lastcolor, bufferRGB + nrline * xDiv8 + imageX - 1, 3)
            for column := imageX; column < xDiv8; column += 1
                Memory.copy(bufferRGB + nrline * xDiv8 + column, &lastcolor, 3)
        }

        imageX  = xDiv8
        tmpline := Memory.alloc(cast(uint) imageX * @sizeof(ColorRGB))

        // Reorder in memory the inversed bitmap
        nrLineUp := imageY - 1
        nrLineDn := 0
        dimline  := cast(uint) (imageX * 3)
        while nrLineUp > nrLineDn
        {
            Memory.copy(tmpline, bufferRGB + nrLineUp * imageX, dimline)
            Memory.copy(bufferRGB + nrLineUp * imageX, bufferRGB + nrLineDn * imageX, dimline)
            Memory.copy(bufferRGB + nrLineDn * imageX, tmpline, dimline)
            nrLineUp -= 1
            nrLineDn += 1
        }

        // Y completion
        Memory.copy(tmpline, bufferRGB + (imageY - 1) * imageX, dimline)
        for nrLine := imageY; nrLine < yDiv8; nrLine += 1
            Memory.copy(bufferRGB + nrLine * imageX, tmpline, dimline)

        imageY = yDiv8
        Memory.free(tmpline, cast(uint) imageX * @sizeof(ColorRGB))
    }
}

public impl IEncoder for Encoder
{
    method canEncode(fileName: string)->bool
    {
        ext := Path.getExtensionLowerCase(fileName)
        return ext == ".jpg" or ext == ".jpeg"
    }

    method encode(result: *ConcatBuffer, image: Image, options: EncodeOptions) throw
    {
        loadBitmap(image)
    }
}