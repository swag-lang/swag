#global marked
#global namespace Jpg
using Core

private
{
    struct SOF0InfoType
    {
        marker:            u16
        length:            u16
        precision:         u8
        height:            u16
        width:             u16
        numComponents:     u8

        idy:               u8
        hvy:               u8
        qty:               u8
        idcb:              u8
        hvcb:              u8
        qtcb:              u8
        idcr:              u8
        hvcr:              u8
        qtcr:              u8
    }

    struct SOSInfoType
    {
        marker:            u16
        length:            u16
        numComponents:     u8

        idy:               u8
        hty:               u8
        idcb:              u8
        htcb:              u8
        idcr:              u8
        htcr:              u8
        ss:                u8
        se:                u8
        bf:                u8
    }

    struct ColorRGB { b, g, r: u8 }

    struct BitString
    {
        length:     u8
        value:      u16
    }

    struct DQTInfoType
    {
        marker:       u16
        length:       u16
        infoQty:      u8
        tableY:       [64] u8
        infoQtcb:     u8
        tableCb:      [64] u8
    }

    struct DHTInfoType
    {
        marker:          u16
        length:          u16
        infoHTYDC:       u8
        nrcodesYDC:      [16] u8
        valuesYDC:       [12] u8
        infoHTYAC:       u8
        nrcodesYAC:      [16] u8
        valuesYAC:       [162] u8
        infoHTCbDC:      u8
        nrcodesCbDC:     [16] u8
        valuesCbDC:      [12] u8
        infoHTCbAC:      u8
        nrcodesCbAC:     [16] u8
        valuesCbAC:      [162] u8
    }

    const SOF0info = SOF0InfoType{0xFFC0, 17, 8, 0, 0, 3, 1, 0x11, 0, 2, 0x11, 1, 3, 0x11, 1}
    const SOSinfo  = SOSInfoType{0xFFDA, 12, 3, 1, 0, 2, 0x11, 3, 0x11, 0, 0x3F, 0}

    const StdDcLuminanceNrCodes   = [0'u8, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0]
    const StdDcLuminanceValues    = [0'u8, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
    const StdDcChrominanceNrCodes = [0'u8, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0]
    const StdDcChrominanceValues  = [0'u8, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
    const StdAcLuminanceNrCodes   = [0'u8, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d]
    const StdAcChrominanceNrCodes = [0'u8, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77]
    const Mask                    = [1'u16, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768]

    const ZigZag: [] u8 = [
        0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42, 3, 8, 12, 17, 25, 30,
        41, 43, 9, 11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38,
        46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63]

    const LuminanceQt: [] u8 = [
        16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99]

    const ChrominanceQt: [] u8 = [
        17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99]

    const StdAcLuminanceValues: [] u8 = [
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
        0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
        0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
        0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa]

    const StdAcChrominanceValues: [] u8 = [
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
        0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
        0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
        0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
        0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa]

    var TabYR:  [256] s32 = undefined
    var TabYG:  [256] s32 = undefined
    var TabYB:  [256] s32 = undefined
    var TabCbR: [256] s32 = undefined
    var TabCbG: [256] s32 = undefined
    var TabCbB: [256] s32 = undefined
    var TabCrR: [256] s32 = undefined
    var TabCrG: [256] s32 = undefined
    var TabCrB: [256] s32 = undefined
}

// Precalculate YCbCr tables
#run
{
    for r in 256
    {
        TabYR[r]  = cast(s32) (65536 * 0.299 + 0.5) * r
        TabCbR[r] = cast(s32) (65536 * -0.16874 + 0.5) * r
        TabCrR[r] = cast(s32) 32768 * r

        TabYG[r]  = cast(s32) (65536 * 0.587 + 0.5) * r
        TabCbG[r] = cast(s32) (65536 * -0.33126 + 0.5) * r
        TabCrG[r] = cast(s32) (65536 * -0.41869 + 0.5) * r

        TabYB[r]  = cast(s32) (65536 * 0.114 + 0.5) * r
        TabCbB[r] = cast(s32) 32768 * r
        TabCrB[r] = cast(s32) (65536 * -0.08131 + 0.5) * r
    }
}

public struct EncodeOptions
{
    quality: u8 = 95 // Encoding quality between 1 and 100
}

#[Swag.Opaque]
public struct Encoder
{
    imageX:            u16
    imageY:            u16
    bufferRGB:         Array'ColorRGB

    stream:            *ConcatBuffer
    srcImage:          const *Image

    bytenew:           u8
    bytepos:           s8

    scaleFactor:       u8 = 10

    dqtInfo:           DQTInfoType
    dhtInfo:           DHTInfoType
    htYDC:             [12] BitString
    htCbDC:            [12] BitString
    htYAC:             [256] BitString
    htCbAC:            [256] BitString

    fdtblY:            [64] f32
    fdtblCb:           [64] f32

    duY:               [64] s8
    duCb:              [64] s8
    duCr:              [64] s8
    duDct:             [64] s16
    du:                [64] s16

    categoryAlloc:     Array'u8
    category:          ^u8
    bitcodeAlloc:      Array'BitString
    bitcode:           ^BitString
}

impl Encoder
{
    mtd loadBitmap(image: Image)
    {
        me.imageX = cast(u16) image.width
        me.imageY = cast(u16) image.height

        var xDiv8, yDiv8: u32
        if me.imageX % 8 != 0:
            xDiv8 = (me.imageX / 8) * 8'u32 + 8
        else:
            xDiv8 = me.imageX
        if me.imageY % 8 != 0:
            yDiv8 = (me.imageY / 8) * 8'u32 + 8
        else:
            yDiv8 = me.imageY

        me.bufferRGB.resize(cast(u64) xDiv8 * yDiv8)
        let pdst = me.bufferRGB.buffer
        var pref = image.pixels.buffer

        for nrline in me.imageY
        {
            switch image.bpp
            {
            case 24:
                Memory.copy(pdst + nrline * xDiv8, pref, me.imageX * 3'u64)
                pref += me.imageX * 3'u64

            case 32:
                for column in me.imageX
                {
                    pdst[column + nrline * xDiv8].b = pref[0]
                    pdst[column + nrline * xDiv8].g = pref[1]
                    pdst[column + nrline * xDiv8].r = pref[2]
                    pref += 4
                }
            }

            var lastcolor: ColorRGB = undefined
            Memory.copy(&lastcolor, pdst + nrline * xDiv8 + me.imageX - 1, 3)
            for var column = me.imageX; column < xDiv8; column += 1:
                Memory.copy(pdst + nrline * xDiv8 + column, &lastcolor, 3)
        }

        me.imageX = cast(u16) xDiv8
        var tmpline = Array.createBuffer(cast(u64) me.imageX * #sizeof(ColorRGB))

        // Reorder in memory the inversed bitmap
        var nrLineUp = cast(u32) me.imageY - 1
        var nrLineDn = 0'u32
        let dimline  = cast(u32) me.imageX * 3'u64
        while nrLineUp > nrLineDn
        {
            Memory.copy(tmpline.buffer, pdst + nrLineUp * me.imageX, dimline)
            Memory.copy(pdst + nrLineUp * me.imageX, pdst + nrLineDn * me.imageX, dimline)
            Memory.copy(pdst + nrLineDn * me.imageX, tmpline.buffer, dimline)
            nrLineUp -= 1
            nrLineDn += 1
        }

        // Y completion
        Memory.copy(tmpline.buffer, pdst + (me.imageY - 1) * cast(u32) me.imageX, dimline)
        for var nrLine = me.imageY; nrLine < yDiv8; nrLine += 1:
            Memory.copy(pdst + nrLine * cast(u32) me.imageX, tmpline.buffer, dimline)

        me.imageY = cast(u16) yDiv8
    }

    mtd setQuantTable(basicTable: const ^u8, newTable: ^u8)
    {
        for i in 64
        {
            var temp = (cast(s32) basicTable[i] * me.scaleFactor + 50) / 100
            temp                = Math.clamp(temp, 1, 255)
            newTable[ZigZag[i]] = cast(u8) temp
        }
    }

    mtd setDQTinfo()
    {
        me.dqtInfo.marker   = 0xFFDB
        me.dqtInfo.length   = 132
        me.dqtInfo.infoQty  = 0
        me.dqtInfo.infoQtcb = 1
        me.setQuantTable(LuminanceQt, me.dqtInfo.tableY)
        me.setQuantTable(ChrominanceQt, me.dqtInfo.tableCb)
    }

    mtd setDHTinfo()
    {
        me.dhtInfo.marker     = 0xFFC4
        me.dhtInfo.length     = 0x01A2
        me.dhtInfo.infoHTYDC  = 0
        me.dhtInfo.infoHTYAC  = 0x10
        me.dhtInfo.infoHTCbDC = 1
        me.dhtInfo.infoHTCbAC = 0x11

        for i in 16:
            me.dhtInfo.nrcodesYDC[i] = StdDcLuminanceNrCodes[i + 1]
        for i in 12:
            me.dhtInfo.valuesYDC[i] = StdDcLuminanceValues[i]
        for i in 16:
            me.dhtInfo.nrcodesYAC[i] = StdAcLuminanceNrCodes[i + 1]
        for i in 162:
            me.dhtInfo.valuesYAC[i] = StdAcLuminanceValues[i]
        for i in 16:
            me.dhtInfo.nrcodesCbDC[i] = StdDcChrominanceNrCodes[i + 1]
        for i in 12:
            me.dhtInfo.valuesCbDC[i] = StdDcChrominanceValues[i]
        for i in 16:
            me.dhtInfo.nrcodesCbAC[i] = StdAcChrominanceNrCodes[i + 1]
        for i in 162:
            me.dhtInfo.valuesCbAC[i] = StdAcChrominanceValues[i]
    }

    func computeHuffmanTable(nrCodes: const ^u8, stdTable: const ^u8, hT: ^BitString)
    {
        var codevalue  = 0'u16
        var posInTable = 0
        for k in 1 to 16
        {
            for var j = 1; j <= nrCodes[k]; j += 1
            {
                hT[stdTable[posInTable]].value  = codevalue
                hT[stdTable[posInTable]].length = cast(u8) k
                posInTable += 1
                codevalue += #wrap 1
            }

            codevalue *= #wrap 2
        }
    }

    mtd initHuffmanTables()
    {
        computeHuffmanTable(StdDcLuminanceNrCodes, StdDcLuminanceValues, me.htYDC)
        computeHuffmanTable(StdDcChrominanceNrCodes, StdDcChrominanceValues, me.htCbDC)
        computeHuffmanTable(StdAcLuminanceNrCodes, StdAcLuminanceValues, me.htYAC)
        computeHuffmanTable(StdAcChrominanceNrCodes, StdAcChrominanceValues, me.htCbAC)
    }

    mtd setNumbersCategoryAndBitcode()
    {
        me.categoryAlloc.resize(65535)
        me.bitcodeAlloc.resize(65535)
        me.category = me.categoryAlloc.buffer + 32767
        me.bitcode  = me.bitcodeAlloc.buffer + 32767

        var nrlower = 1
        var nrupper = 2
        for cat in 1 to 15
        {
            for nr in nrlower until nrupper
            {
                me.category[nr]       = cast(u8) cat
                me.bitcode[nr].length = cast(u8) cat
                me.bitcode[nr].value  = cast(u16) nr
            }

            for nr in -(nrupper - 1) to -nrlower
            {
                let nof = cast #bit (u64) cast(s64) nr
                me.category[nof]       = cast(u8) cat
                me.bitcode[nof].length = cast(u8) cat
                me.bitcode[nof].value  = cast(u16) (nrupper - 1 + nr)
            }

            nrlower <<= 1
            nrupper <<= 1
        }
    }

    mtd prepareQuantTables()
    {
        const AanSf: [8] f64 = [1.0, 1.387039845, 1.306562965, 1.175875602, 1.0, 0.785694958, 0.541196100, 0.275899379]
        var i        = 0
        for row in 8
        {
            for col in 8
            {
                me.fdtblY[i]  = cast(f32) (1.0 / (cast(f64) me.dqtInfo.tableY[ZigZag[i]] * AanSf[row] * AanSf[col] * 8.0))
                me.fdtblCb[i] = cast(f32) (1.0 / (cast(f64) me.dqtInfo.tableCb[ZigZag[i]] * AanSf[row] * AanSf[col] * 8.0))
                i += 1
            }
        }
    }

    #[Swag.Inline]
    mtd writeByte(b: u8)
    {
        me.stream.addNative(b)
    }

    #[Swag.Inline]
    mtd writeWord(w: u16)
    {
        me.stream.addNative(cast(u8) (w >> 8))
        me.stream.addNative(cast(u8) (w & 0xFF))
    }

    #[Swag.Inline]
    mtd writeBytes(s: [..] u8)
    {
        me.stream.addBytes(s)
    }

    mtd writeBits(bs: BitString)
    {
        let value  = bs.value
        var posval = cast(s32) bs.length - 1
        while posval >= 0
        {
            if value & Mask[posval]:
                me.bytenew |= cast(u8) Mask[me.bytepos]
            posval -= 1
            me.bytepos -= 1
            if me.bytepos < 0
            {
                if me.bytenew == 0xFF
                {
                    me.writeByte(0xFF)
                    me.writeByte(0)
                }
                else
                {
                    me.writeByte(me.bytenew)
                }

                me.bytepos = 7
                me.bytenew = 0
            }
        }
    }

    mtd writeMetaDatas()
    {
        foreach md in me.srcImage.metaDatas
        {
            me.writeWord(0xFFE0'u16 + cast(u16) md.sig)
            me.writeWord(3 + cast(u16) md.tag.count + cast(u16) md.value.count)
            me.writeBytes(md.tag.toSlice())
            me.writeByte(0)
            me.writeBytes(md.value.toSlice())
        }
    }

    mtd writeDQTinfo()
    {
        me.writeWord(me.dqtInfo.marker)
        me.writeWord(me.dqtInfo.length)
        me.writeByte(me.dqtInfo.infoQty)
        me.writeBytes(me.dqtInfo.tableY)
        me.writeByte(me.dqtInfo.infoQtcb)
        me.writeBytes(me.dqtInfo.tableCb)
    }

    mtd writeSOF0info()
    {
        me.writeWord(SOF0info.marker)
        me.writeWord(SOF0info.length)
        me.writeByte(SOF0info.precision)
        me.writeWord(cast(u16) me.srcImage.height)
        me.writeWord(cast(u16) me.srcImage.width)
        me.writeByte(SOF0info.numComponents)
        me.writeByte(SOF0info.idy)
        me.writeByte(SOF0info.hvy)
        me.writeByte(SOF0info.qty)
        me.writeByte(SOF0info.idcb)
        me.writeByte(SOF0info.hvcb)
        me.writeByte(SOF0info.qtcb)
        me.writeByte(SOF0info.idcr)
        me.writeByte(SOF0info.hvcr)
        me.writeByte(SOF0info.qtcr)
    }

    mtd writeDHTinfo()
    {
        me.writeWord(me.dhtInfo.marker)
        me.writeWord(me.dhtInfo.length)
        me.writeByte(me.dhtInfo.infoHTYDC)
        me.writeBytes(me.dhtInfo.nrcodesYDC)
        me.writeBytes(me.dhtInfo.valuesYDC)
        me.writeByte(me.dhtInfo.infoHTYAC)
        me.writeBytes(me.dhtInfo.nrcodesYAC)
        me.writeBytes(me.dhtInfo.valuesYAC)
        me.writeByte(me.dhtInfo.infoHTCbDC)
        me.writeBytes(me.dhtInfo.nrcodesCbDC)
        me.writeBytes(me.dhtInfo.valuesCbDC)
        me.writeByte(me.dhtInfo.infoHTCbAC)
        me.writeBytes(me.dhtInfo.nrcodesCbAC)
        me.writeBytes(me.dhtInfo.valuesCbAC)
    }

    mtd writeSOSinfo()
    {
        me.writeWord(SOSinfo.marker)
        me.writeWord(SOSinfo.length)
        me.writeByte(SOSinfo.numComponents)
        me.writeByte(SOSinfo.idy)
        me.writeByte(SOSinfo.hty)
        me.writeByte(SOSinfo.idcb)
        me.writeByte(SOSinfo.htcb)
        me.writeByte(SOSinfo.idcr)
        me.writeByte(SOSinfo.htcr)
        me.writeByte(SOSinfo.ss)
        me.writeByte(SOSinfo.se)
        me.writeByte(SOSinfo.bf)
    }

    mtd loadFromRGB(xpos: u16, ypos: u16)
    {
        #[Swag.Inline]
        {
            func Y(r, g, b: u8)  => cast(s8) (((TabYR[r] + TabYG[g] + TabYB[b]) >> 16) - 128)
            func Cb(r, g, b: u8) => cast(s8) ((TabCbR[r] + TabCbG[g] + TabCbB[b]) >> 16)
            func Cr(r, g, b: u8) => cast(s8) ((TabCrR[r] + TabCrG[g] + TabCrB[b]) >> 16)
        }

        var pos      = 0
        var location = cast(s32) ypos * me.imageX + xpos
        let pdst     = me.bufferRGB.buffer

        for y in 8
        {
            for x in 8
            {
                let r = pdst[location].r
                let g = pdst[location].g
                let b = pdst[location].b
                me.duY[pos]  = Y(r, g, b)
                me.duCb[pos] = Cb(r, g, b)
                me.duCr[pos] = Cr(r, g, b)
                location += 1
                pos += 1
            }

            location += me.imageX - 8
        }
    }

    func quantization(data: ^s8, fdtbl: ^f32, outData: ^s16)
    {
        var dataFloat: [64] f32
        for i in 64:
            dataFloat[i] = data[i]

        var dataptr = &dataFloat
        for #reverse ctr in 0 to 7
        {
            let tmp0 = dataptr[0] + dataptr[7]
            let tmp7 = dataptr[0] - dataptr[7]
            let tmp1 = dataptr[1] + dataptr[6]
            let tmp6 = dataptr[1] - dataptr[6]
            let tmp2 = dataptr[2] + dataptr[5]
            let tmp5 = dataptr[2] - dataptr[5]
            let tmp3 = dataptr[3] + dataptr[4]
            let tmp4 = dataptr[3] - dataptr[4]

            var tmp10 = tmp0 + tmp3
            let tmp13 = tmp0 - tmp3
            var tmp11 = tmp1 + tmp2
            var tmp12 = tmp1 - tmp2

            dataptr[0] = tmp10 + tmp11
            dataptr[4] = tmp10 - tmp11

            let z1 = (tmp12 + tmp13) * 0.707106781
            dataptr[2] = tmp13 + z1
            dataptr[6] = tmp13 - z1

            tmp10 = tmp4 + tmp5
            tmp11 = tmp5 + tmp6
            tmp12 = tmp6 + tmp7

            let z5 = (tmp10 - tmp12) * 0.382683433
            let z2 = 0.541196100 * tmp10 + z5
            let z4 = 1.306562965 * tmp12 + z5
            let z3 = tmp11 * 0.707106781

            let z11 = tmp7 + z3
            let z13 = tmp7 - z3

            dataptr[5] = z13 + z2
            dataptr[3] = z13 - z2
            dataptr[1] = z11 + z4
            dataptr[7] = z11 - z4

            dataptr += 8
        }

        dataptr = dataFloat

        for #reverse ctr in 0 to 7
        {
            let tmp0 = dataptr[0] + dataptr[56]
            let tmp7 = dataptr[0] - dataptr[56]
            let tmp1 = dataptr[8] + dataptr[48]
            let tmp6 = dataptr[8] - dataptr[48]
            let tmp2 = dataptr[16] + dataptr[40]
            let tmp5 = dataptr[16] - dataptr[40]
            let tmp3 = dataptr[24] + dataptr[32]
            let tmp4 = dataptr[24] - dataptr[32]

            var tmp10 = tmp0 + tmp3
            let tmp13 = tmp0 - tmp3
            var tmp11 = tmp1 + tmp2
            var tmp12 = tmp1 - tmp2

            dataptr[0]  = tmp10 + tmp11
            dataptr[32] = tmp10 - tmp11

            let z1 = (tmp12 + tmp13) * 0.707106781
            dataptr[16] = tmp13 + z1
            dataptr[48] = tmp13 - z1

            tmp10 = tmp4 + tmp5
            tmp11 = tmp5 + tmp6
            tmp12 = tmp6 + tmp7

            let z5 = (tmp10 - tmp12) * 0.382683433
            let z2 = 0.541196100 * tmp10 + z5
            let z4 = 1.306562965 * tmp12 + z5
            let z3 = tmp11 * 0.707106781

            let z11 = tmp7 + z3
            let z13 = tmp7 - z3

            dataptr[40] = z13 + z2
            dataptr[24] = z13 - z2
            dataptr[8]  = z11 + z4
            dataptr[56] = z11 - z4

            dataptr += 1
        }

        for i in 64
        {
            let temp = dataFloat[i] * fdtbl[i]
            outData[i] = cast(s16) (cast(s16) (temp + 16384.5) - 16384)
        }
    }

    mtd processDU(componentDU: ^s8, fdtbl: ^f32, dC: *s16, hTDC, hTAC: ^BitString)
    {
        var eob       = hTAC[0x00]
        var m16Zeroes = hTAC[0xF0]

        quantization(componentDU, fdtbl, me.duDct)

        for i in 64:
            me.du[ZigZag[i]] = me.duDct[i]
        let diff = me.du[0] - dref dC
        dref dC = me.du[0]

        if diff == 0:
            me.writeBits(hTDC[0])
        else
        {
            let diffc = cast #bit (u64) cast(s64) diff // If negative
            me.writeBits(hTDC[me.category[diffc]])
            me.writeBits(me.bitcode[diffc])
        }

        var end0pos = 63
        while (end0pos > 0) and (me.du[end0pos] == 0):
            end0pos -= 1

        if end0pos == 0
        {
            me.writeBits(eob)
            return
        }

        var i = 1
        while i <= end0pos
        {
            let startpos = i
            while (i <= end0pos) and (me.du[i] == 0):
                i += 1
            var nrzeroes = i - startpos
            if nrzeroes >= 16
            {
                for nrmarker in 1 to nrzeroes / 16:
                    me.writeBits(m16Zeroes)
                nrzeroes = nrzeroes % 16
            }

            let offc = cast #bit (u64) cast(s64) me.du[i] // If negative
            me.writeBits(hTAC[nrzeroes * 16 + me.category[offc]])
            me.writeBits(me.bitcode[offc])
            i += 1
        }

        if end0pos != 63:
            me.writeBits(eob)
    }

    mtd encoder()
    {
        var dcy, dccb, dccr: s16
        for var ypos = 0'u16; ypos < me.imageY; ypos += 8
        {
            for var xpos = 0'u16; xpos < me.imageX; xpos += 8
            {
                me.loadFromRGB(xpos, ypos)
                me.processDU(me.duY, me.fdtblY, &dcy, me.htYDC, me.htYAC)
                me.processDU(me.duCb, me.fdtblCb, &dccb, me.htCbDC, me.htCbAC)
                me.processDU(me.duCr, me.fdtblCb, &dccr, me.htCbDC, me.htCbAC)
            }
        }
    }
}

public impl IImageEncoder for Encoder
{
    mtd impl canEncode(fileName: string)->bool
    {
        var ext = Path.getExtensionLowerCase(fileName)
        return ext == ".jpg" or ext == ".jpeg"
    }

    mtd impl encode(result: *ConcatBuffer, image: Image, options: any) throw
    {
        var opt: EncodeOptions
        if @dataof(options) != null
        {
            Debug.assert(@kindof(options) == EncodeOptions)
            opt = dref cast(*EncodeOptions) @dataof(options)
        }

        me.stream      = result
        me.srcImage    = &image
        me.scaleFactor = cast(u8) (100 - Math.clamp(opt.quality, 1, 100))

        me.loadBitmap(image)

        me.setDQTinfo()
        me.setDHTinfo()
        me.initHuffmanTables()
        me.setNumbersCategoryAndBitcode()
        me.prepareQuantTables()

        me.writeWord(0xFFD8) // SOI

        me.writeMetaDatas()
        me.writeDQTinfo()
        me.writeSOF0info()
        me.writeDHTinfo()
        me.writeSOSinfo()

        me.bytenew = 0
        me.bytepos = 7
        me.encoder()

        if me.bytepos >= 0
        {
            var fillbits: BitString
            fillbits.length = cast(u8) (me.bytepos + 1)
            fillbits.value  = cast(u16) ((1 << (me.bytepos + 1)) - 1)
            me.writeBits(fillbits)
        }

        me.writeWord(0xFFD9) // EOI
    }
}
