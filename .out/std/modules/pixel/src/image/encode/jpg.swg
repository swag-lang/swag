#global namespace Jpg
using Core

struct APP0InfoType
{
    marker:         u16
    length:         u16
    jfifSignature:  [5] u8
    versionHi:      u8
    versionLo:      u8
    xyUnits:        u8
    xDensity:       u16
    yDensity:       u16
    thumbnWidth:    u8
    thumbnHeight:   u8
}

struct SOF0InfoType
{
    marker:     u16
    length:     u16
    precision:  u8
    height:     u16
    width:      u16
    nrofComponents: u8
    idy:    u8
    hvy:    u8
    qty:    u8
    idcb:   u8
    hvcb:   u8
    qtcb:   u8
    idcr:   u8
    hvcr:   u8
    qtcr:   u8
}

struct SOSInfoType
{
    marker: u16
    length: u16
    nrofcomponents: u8
    idy:    u8
    hty:    u8
    idcb:   u8
    htcb:   u8
    idcr:   u8
    htcr:   u8
    ss:     u8
    se:     u8
    bf:     u8
}

struct ColorRGB
{
    b, g, r: u8
}

struct bitstring
{
    length: u8
    value:  u16
}

struct DQTInfoType
{
    marker:   u16
    length:   u16
    infoQty:  u8
    tableY:   [64] u8
    infoQtcb: u8
    tableCb:  [64] u8
}

struct DHTInfoType
{
    marker:         u16
    length:         u16
    HTYDCinfo:      u8
    YDC_nrcodes:    [16] u8
    YDC_values:     [12] u8
    HTYACinfo:      u8
    YAC_nrcodes:    [16] u8
    YAC_values:     [162] u8
    HTCbDCinfo:     u8
    CbDC_nrcodes:   [16] u8
    CbDC_values:    [12] u8
    HTCbACinfo:     u8
    CbAC_nrcodes:   [16] u8
    CbAC_values:    [162] u8
}

const APP0info = APP0InfoType{0xFFE0, 16, @["J"'u8, "F"'u8, "I"'u8, "F"'u8, 0], 1, 1, 0, 1, 1, 0, 0}
const SOF0info = SOF0InfoType{0xFFC0, 17, 8, 0, 0, 3, 1, 0x11, 0, 2, 0x11, 1, 3, 0x11, 1}
const SOSinfo  = SOSInfoType{0xFFDA, 12, 3, 1, 0, 2, 0x11, 3, 0x11, 0, 0x3F, 0}

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

const LuminanceQt: [] u8 = @[
    16, 11, 10, 16, 24, 40,  51,  61, 12, 12, 14, 19, 26, 58,  60,  55, 14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99]

const ChrominanceQt: [] u8 = @[
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

    stream:     *ConcatBuffer
    srcImage:   const *Image

    bytenew: u8
    bytepos: s8

    scaleFactor: u8 = 10

    dqtInfo:    DQTInfoType
    dhtInfo:    DHTInfoType
    YDC_HT:     [12] bitstring
    CbDC_HT:    [12] bitstring
    YAC_HT:     [256] bitstring
    CbAC_HT:    [256] bitstring

    YRtab:  [256] s32
    YGtab:  [256] s32
    YBtab:  [256] s32
    CbRtab: [256] s32
    CbGtab: [256] s32
    CbBtab: [256] s32
    CrRtab: [256] s32
    CrGtab: [256] s32
    CrBtab: [256] s32
    fdtbl_Y:  [64] f32
    fdtbl_Cb: [64] f32

    category_alloc: Array'u8
    category:       *u8
    bitcode_alloc:  Array'bitstring
    bitcode:        *bitstring
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
        tmpline := Array.createBuffer(cast(uint) imageX * @sizeof(ColorRGB))

        // Reorder in memory the inversed bitmap
        nrLineUp := imageY - 1
        nrLineDn := 0
        dimline  := cast(uint) (imageX * 3)
        while nrLineUp > nrLineDn
        {
            Memory.copy(tmpline.buffer, bufferRGB + nrLineUp * imageX, dimline)
            Memory.copy(bufferRGB + nrLineUp * imageX, bufferRGB + nrLineDn * imageX, dimline)
            Memory.copy(bufferRGB + nrLineDn * imageX, tmpline, dimline)
            nrLineUp -= 1
            nrLineDn += 1
        }

        // Y completion
        Memory.copy(tmpline.buffer, bufferRGB + (imageY - 1) * imageX, dimline)
        for nrLine := imageY; nrLine < yDiv8; nrLine += 1
            Memory.copy(bufferRGB + nrLine * imageX, tmpline, dimline)

        imageY = yDiv8
    }

    func setQuantTable(_basic_table: const *u8, _scale_factor: u8, _newtable: *u8)
    {
        loop i: 64
        {
            temp := (cast(s32) _basic_table[i] * _scale_factor + 50) / 100
            temp = Math.clamp(temp, 0, 255)
            _newtable[zigzag[i]] = cast(u8) temp
        }
    }

    method setDQTinfo()
    {
        dqtInfo.marker   = 0xFFDB
        dqtInfo.length   = 132
        dqtInfo.infoQty  = 0
        dqtInfo.infoQtcb = 1
        setQuantTable(LuminanceQt, scaleFactor, dqtInfo.tableY)
        setQuantTable(ChrominanceQt, scaleFactor, dqtInfo.tableCb)
    }

    method setDHTinfo()
    {
        dhtInfo.marker     = 0xFFC4
        dhtInfo.length     = 0x01A2
        dhtInfo.HTYDCinfo  = 0
        dhtInfo.HTYACinfo  = 0x10
        dhtInfo.HTCbDCinfo = 1
        dhtInfo.HTCbACinfo = 0x11

        loop i: 16  dhtInfo.YDC_nrcodes[i] = std_dc_luminance_nrcodes[i + 1]
        loop i: 12  dhtInfo.YDC_values[i] = std_dc_luminance_values[i]
        loop i: 16  dhtInfo.YAC_nrcodes[i] = std_ac_luminance_nrcodes[i + 1]
        loop i: 162 dhtInfo.YAC_values[i] = std_ac_luminance_values[i]
        loop i: 16  dhtInfo.CbDC_nrcodes[i] = std_dc_chrominance_nrcodes[i + 1]
        loop i: 12  dhtInfo.CbDC_values[i] = std_dc_chrominance_values[i]
        loop i: 16  dhtInfo.CbAC_nrcodes[i] = std_ac_chrominance_nrcodes[i + 1]
        loop i: 162 dhtInfo.CbAC_values[i] = std_ac_chrominance_values[i]
    }

    method computeHuffmanTable(_nrcodes: const *u8, _std_table: const *u8, _hT: *bitstring)
    {
        codevalue    := 0'u16
        pos_in_table := 0
        loop k: 1..16
        {
            for j := 1; j <= _nrcodes[k]; j += 1
            {
                _hT[_std_table[pos_in_table]].value  = codevalue
                _hT[_std_table[pos_in_table]].length = cast(u8) k
                pos_in_table += 1
                codevalue +=,safe 1
            }

            codevalue *=,safe 2
        }
    }

    method initHuffmanTables()
    {
        computeHuffmanTable(std_dc_luminance_nrcodes, std_dc_luminance_values, YDC_HT)
        computeHuffmanTable(std_dc_chrominance_nrcodes, std_dc_chrominance_values, CbDC_HT)
        computeHuffmanTable(std_ac_luminance_nrcodes, std_ac_luminance_values, YAC_HT)
        computeHuffmanTable(std_ac_chrominance_nrcodes, std_ac_chrominance_values, CbAC_HT)
    }

    method setNumbersCategoryAndBitcode()
    {
        category_alloc.resize(65535)
        bitcode_alloc.resize(65535)
        category = category_alloc.buffer + 32767
        bitcode  = bitcode_alloc.buffer + 32767

        nrlower := 1
        nrupper := 2
        loop cat: 1..15
        {
            loop nr:  nrlower..nrupper-1
            {
                category[nr]       = cast(u8) cat
                bitcode[nr].length = cast(u8) cat
                bitcode[nr].value  = cast(u16) nr
            }

            loop nr: -(nrupper - 1)..(-nrlower)
            {
                nof := bitcast(uint) cast(int) nr
                category[nof] = cast(u8) cat
                bitcode[nof].length = cast(u8) cat
                bitcode[nof].value  = cast(u16) (nrupper - 1 + nr)
            }

            nrlower <<= 1
            nrupper <<= 1
        }
    }

    method precalculateYCbCrTables()
    {
        loop R: 256
        {
            YRtab[R]  = cast(s32) (65536 * 0.299 + 0.5) * R
            CbRtab[R] = cast(s32) (65536 * -0.16874 + 0.5) * R
            CrRtab[R] = cast(s32) (32768) * R
        }

        loop G: 256
        {
            YGtab[G]  = cast(s32) (65536 * 0.587 + 0.5) * G
            CbGtab[G] = cast(s32) (65536 * -0.33126 + 0.5) * G
            CrGtab[G] = cast(s32) (65536 * -0.41869 + 0.5) * G
        }

        loop B: 256
        {
            YBtab[B]  = cast(s32) (65536 * 0.114 + 0.5) * B
            CbBtab[B] = cast(s32) (32768) * B
            CrBtab[B] = cast(s32) (65536 * -0.08131 + 0.5) * B
        }
    }

    method prepareQuantTables()
    {
        const AanSf: [8] f64 = @[1.0, 1.387039845, 1.306562965, 1.175875602, 1.0, 0.785694958, 0.541196100, 0.275899379]
        i := 0
        loop row: 8
        {
            loop col: 8
            {
                fdtbl_Y[i]  = cast(f32) (1.0 / (cast(f64) dqtInfo.tableY[zigzag[i]] * AanSf[row] * AanSf[col] * 8.0))
                fdtbl_Cb[i] = cast(f32) (1.0 / (cast(f64) dqtInfo.tableCb[zigzag[i]] * AanSf[row] * AanSf[col] * 8.0))
                i += 1
            }
        }
    }

    method writeByte(b: u8)
    {
        stream.addByte(b)
    }

    method writeWord(w: u16)
    {
        stream.addByte(cast(u8) (w >>,safe 8))
        stream.addByte(cast(u8) (w & 0xFF))
    }

    method writeBits(_bs: bitstring)
    {
        value  := _bs.value
        posval := cast(s32) _bs.length - 1
        while posval >= 0
        {
            if value & mask[posval]
                bytenew |= cast(u8) mask[bytepos]
            posval -= 1
            bytepos -= 1
            if bytepos < 0
            {
                if bytenew == 0xFF
                {
                    writeByte(0xFF)
                    writeByte(0)
                }
                else
                {
                    writeByte(bytenew)
                }

                bytepos = 7
                bytenew = 0
            }
        }
    }

    method writeAPP0info()
    {
        writeWord(APP0info.marker)
        writeWord(APP0info.length)
        writeByte("J"'u8)
        writeByte("F"'u8)
        writeByte("I"'u8)
        writeByte("F"'u8)
        writeByte(0)
        writeByte(APP0info.versionHi)
        writeByte(APP0info.versionLo)
        writeByte(APP0info.xyUnits)
        writeWord(APP0info.xDensity)
        writeWord(APP0info.yDensity)
        writeByte(APP0info.thumbnWidth)
        writeByte(APP0info.thumbnHeight)
    }

    method writeDQTinfo()
    {
        writeWord(dqtInfo.marker)
        writeWord(dqtInfo.length)
        writeByte(dqtInfo.infoQty)
        loop i: 64 writeByte(dqtInfo.tableY[i])
        writeByte(dqtInfo.infoQtcb)
        loop i: 64 writeByte(dqtInfo.tableCb[i])
    }

    method writeSOF0info()
    {
        writeWord(SOF0info.marker)
        writeWord(SOF0info.length)
        writeByte(SOF0info.precision)
        writeWord(cast(u16) srcImage.height)
        writeWord(cast(u16) srcImage.width)
        writeByte(SOF0info.nrofComponents)
        writeByte(SOF0info.idy)
        writeByte(SOF0info.hvy)
        writeByte(SOF0info.qty)
        writeByte(SOF0info.idcb)
        writeByte(SOF0info.hvcb)
        writeByte(SOF0info.qtcb)
        writeByte(SOF0info.idcr)
        writeByte(SOF0info.hvcr)
        writeByte(SOF0info.qtcr)
    }

    method writeDHTinfo()
    {
        writeWord(dhtInfo.marker)
        writeWord(dhtInfo.length)
        writeByte(dhtInfo.HTYDCinfo)
        loop i: 16  writeByte(dhtInfo.YDC_nrcodes[i])
        loop i: 12  writeByte(dhtInfo.YDC_values[i])
        writeByte(dhtInfo.HTYACinfo)
        loop i: 16  writeByte(dhtInfo.YAC_nrcodes[i])
        loop i: 162 writeByte(dhtInfo.YAC_values[i])
        writeByte(dhtInfo.HTCbDCinfo)
        loop i: 16  writeByte(dhtInfo.CbDC_nrcodes[i])
        loop i: 12  writeByte(dhtInfo.CbDC_values[i])
        writeByte(dhtInfo.HTCbACinfo)
        loop i: 16  writeByte(dhtInfo.CbAC_nrcodes[i])
        loop i: 162 writeByte(dhtInfo.CbAC_values[i])
    }

    method writeSOSinfo()
    {
        writeWord(SOSinfo.marker)
        writeWord(SOSinfo.length)
        writeByte(SOSinfo.nrofcomponents)
        writeByte(SOSinfo.idy)
        writeByte(SOSinfo.hty)
        writeByte(SOSinfo.idcb)
        writeByte(SOSinfo.htcb)
        writeByte(SOSinfo.idcr)
        writeByte(SOSinfo.htcr)
        writeByte(SOSinfo.ss)
        writeByte(SOSinfo.se)
        writeByte(SOSinfo.bf)
    }

    method load_data_units_from_RGB_buffer(_xpos: u16, _ypos: u16)
    {
    }

    method main_encoder()
    {
        for ypos := 0'u16; ypos < imageY; ypos += 8
        {
            for xpos := 0'u16; xpos < imageX; xpos += 8
            {
                load_data_units_from_RGB_buffer(xpos, ypos)
                //process_DU(YDU, fdtbl_Y, &DCY, YDC_HT, YAC_HT)
                //process_DU(CbDU, fdtbl_Cb, &DCCb, CbDC_HT, CbAC_HT)
                //process_DU(CrDU, fdtbl_Cb, &DCCr, CbDC_HT, CbAC_HT)
            }
        }
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
        stream = result
        srcImage = &image

        loadBitmap(image)

        setDQTinfo()
        setDHTinfo()
        initHuffmanTables()
        setNumbersCategoryAndBitcode()
        precalculateYCbCrTables()
        prepareQuantTables()

        writeWord(0xFFD8) // SOI

        writeAPP0info()
        writeDQTinfo()
        writeSOF0info()
        writeDHTinfo()
        writeSOSinfo()

        bytenew = 0
        bytepos = 7
        main_encoder()

        if (bytepos >= 0)
        {
            var fillbits: bitstring
            fillbits.length = cast(u8) (bytepos + 1)
            fillbits.value  = cast(u16) ((1 << (bytepos + 1)) - 1)
            writeBits(fillbits)
        }


        writeWord(0xFFD9) // EOI
    }
}