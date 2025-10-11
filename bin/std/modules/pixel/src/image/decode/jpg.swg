#global namespace Jpg
using Swag, Core, Core.Errors

private
{
    const InBufSize       = 4096
    const MaxHeight       = 8192
    const MaxWidth        = 8192
    const MaxComponents   = 4
    const MaxQuantTables  = 4
    const MaxHuffTables   = 8
    const MaxBocksPerMcu  = 10
    const MaxBlocksPerRow = 6144
    const MaxCompsInScan  = 4
    const GrayScale       = 0
    const YH1V1           = 1
    const YH2V1           = 2
    const YH1V2           = 3
    const YH2V2           = 4
    const Failed          = -1
    const Done            = 1
    const Okay            = 0
    const ConstBits       = 13'u32
    const Pass1Bits       = 2'u32
    const Fix_0_298631336 = 2446
    const Fix_0_390180644 = 3196
    const Fix_0_541196100 = 4433
    const Fix_0_765366865 = 6270
    const Fix_0_899976223 = 7373
    const Fix_1_175875602 = 9633
    const Fix_1_501321110 = 12299
    const Fix_1_847759065 = 15137
    const Fix_1_961570560 = 16069
    const Fix_2_053119869 = 16819
    const Fix_2_562915447 = 20995
    const Fix_3_072711026 = 25172

    struct HuffTables
    {
        lookUp:       [256] s32
        codeSize:     [256] u8
        tree:         [512] s32
    }

    struct CoeffBuf
    {
        pdata:         Array'u8
        blockNumX:     s32
        blockNumY:     s32
        blockLenX:     s32
        blockLenY:     s32
        blockSize:     s32
    }

    enum Marker
    {
        M_SOF0  = 0xC0
        M_SOF1  = 0xC1
        M_SOF2  = 0xC2
        M_SOF3  = 0xC3
        M_SOF5  = 0xC5
        M_SOF6  = 0xC6
        M_SOF7  = 0xC7
        M_JPG   = 0xC8
        M_SOF9  = 0xC9
        M_SOF10 = 0xCA
        M_SOF11 = 0xCB
        M_SOF13 = 0xCD
        M_SOF14 = 0xCE
        M_SOF15 = 0xCF
        M_DHT   = 0xC4
        M_DAC   = 0xCC
        M_RST0  = 0xD0
        M_RST1  = 0xD1
        M_RST2  = 0xD2
        M_RST3  = 0xD3
        M_RST4  = 0xD4
        M_RST5  = 0xD5
        M_RST6  = 0xD6
        M_RST7  = 0xD7
        M_SOI   = 0xD8
        M_EOI   = 0xD9
        M_SOS   = 0xDA
        M_DQT   = 0xDB
        M_DNL   = 0xDC
        M_DRI   = 0xDD
        M_DHP   = 0xDE
        M_EXP   = 0xDF
        M_APP0  = 0xE0
        M_APP15 = 0xEF
        M_JPG0  = 0xF0
        M_JPG13 = 0xFD
        M_COM   = 0xFE
        M_TEM   = 0x01
        M_ERROR = 0x100
    }

    const ExtendOff: [16] s32 = [
        0,
        (-1 << 1) + 1, (-1 << 2) + 1, (-1 << 3) + 1, (-1 << 4) + 1,
        (-1 << 5) + 1, (-1 << 6) + 1, (-1 << 7) + 1, (-1 << 8) + 1,
        (-1 << 9) + 1, (-1 << 10) + 1, (-1 << 11) + 1, (-1 << 12) + 1,
        (-1 << 13) + 1, (-1 << 14) + 1, (-1 << 15) + 1]

    const ExtendTest: [16] s32 = [
        0, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020,
        0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800,
        0x1000, 0x2000, 0x4000]

    const Zag: [64] u32 = [
        0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
        12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21,
        28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37,
        44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47,
        55, 62, 63]

    #[Inline]
    {
        func descale(x: s32, n: u32)->s16 => cast(s16) ((x + (1's32 << (n - 1))) >> n)
        func rol(i, j: u32)->u32          => cast(u32) ((i << j) | (i >> (32 - j)))
        func clamp(i: s32)->u8            => cast(u8) ((i & cast #bit (s32) 0xFFFFFF00) ? ((~i >> 31) & 0xFF) : i)
    }
}

#[Opaque]
public struct Decoder
{
    stream:                       const [*] u8
    inBufOfs:                     [*] u8
    pSampleBuf:                   [*] u8

    imageSizeX:                   s32
    imageSizeY:                   s32
    streamLen:                    s32
    streamSeek:                   s32
    bitBuf:                       u32
    bitsLeft:                     s32
    inBufLeft:                    s32
    temFlag:                      s32
    eofReached:                   s32
    totalBytesRead:               s32
    compsInFrame:                 s32
    restartInterval:              s32
    scanType:                     s32
    maxBlocksPerMcu:              s32
    maxMcuSizeX:                  s32
    maxMcuSizeY:                  s32
    blocksPerMcu:                 u32
    maxMcusPerRow:                s32
    maxMcusPerCol:                s32
    maxBlocksPerRow:              s32
    totalLinesLeft:               s32
    mcuLinesLeft:                 s32
    spectralStart:                s32
    spectralEnd:                  s32
    successiveLow:                s32
    successiveHigh:               s32
    mcusPerRow:                   s32
    mcusPerCol:                   s32
    eobRun:                       s32
    restartsLeft:                 s32
    nextRestartNum:               s32
    compsInScan:                  u32

    destBytesPerPixel:            s32
    realDestBytesPerScanLine:     s32
    destBytesPerScanLine:         s32

    acceptMetaDatas:              bool
    progressiveFlag:              bool

    inBuf:                        [InBufSize + 128] u8
    compSampH:                    [MaxComponents] s32
    compSampV:                    [MaxComponents] s32
    compQuant:                    [MaxComponents] u32
    compIdent:                    [MaxComponents] s32
    blockMaxZagSet:               [MaxBlocksPerRow] s32
    blockSeg:                     [MaxBlocksPerRow] [*] s16
    compList:                     [MaxCompsInScan] u32
    compDcTab:                    [MaxComponents] s32
    compAcTab:                    [MaxComponents] s32
    compBlocksH:                  [MaxComponents] s32
    compBlocksV:                  [MaxComponents] s32
    mcuOrg:                       [MaxBocksPerMcu] u32
    h:                            [MaxHuffTables] HuffTables
    dcHuffSeg:                    [MaxBocksPerMcu] *HuffTables
    acHuffSeg:                    [MaxBocksPerMcu] *HuffTables
    blockMcuY:                    [MaxComponents] s32
    component:                    [MaxBocksPerMcu] *u32
    lastDcVal:                    [MaxComponents] s32
    dcCoeffs:                     [MaxComponents] CoeffBuf
    acCoeffs:                     [MaxComponents] CoeffBuf
    tempBlock:                    [64] s16
    crr:                          [256] s32
    cbb:                          [256] s32
    crg:                          [256] s32
    cbg:                          [256] s32

    huffNum:                      [MaxHuffTables] Array'u8
    huffVal:                      [MaxHuffTables] Array'u8
    quant:                        [MaxQuantTables] Array's16
    scanLine0:                    Array'u8
    scanLine1:                    Array'u8
    quantBuf:                     Array'u8
    sampleBuf:                    Array'u8
    decOptions:                   DecodeOptions
    metaDatas:                    Array'ImageMetaData
}

impl Decoder
{
    mtd init(buffer: const [..] u8) throw
    {
        @init(me, 1)
        .stream    = @dataof(buffer)
        .streamLen = cast(s32) @countof(buffer)

        .inBufOfs = .inBuf
        .prepareInBuffer()
        .bitsLeft = 16
        .getBits1(16)
        .getBits1(16)
        for i in MaxBlocksPerRow do
            .blockMaxZagSet[i] = 64

        .locateSofMarker()
    }

    mtd start() throw
    {
        .initFrame()
        if .progressiveFlag do
            .initProgressive()
        else do
            .initSequential()
    }

    mtd doit(buf: [*] u8) throw
    {
        var scanLineOfs: *void
        var scanLineLen: u32

        // Gray scale (1BPP)
        if .compsInFrame == 1
        {
            var pdest = buf
            pdest += (.imageSizeY - 1) * .imageSizeX * 3
            for
            {
                if .decode(&scanLineOfs, &scanLineLen) do
                    break

                var pdest1 = pdest
                var psrc   = cast([*] u8) scanLineOfs
                for .imageSizeX
                {
                    pdest1[0] = dref psrc
                    pdest1[1] = dref psrc
                    pdest1[2] = dref psrc
                    pdest1 += 3
                    psrc += 1
                }

                pdest -= .imageSizeX * 3
            }
        }

        // 3 BPP
        else
        {
            var pdest = buf
            pdest += (.imageSizeY - 1) * .imageSizeX * 3
            for
            {
                if .decode(&scanLineOfs, &scanLineLen) do
                    break
                Memory.copy(pdest, scanLineOfs, cast(u64) .imageSizeX * 3)
                pdest -= .imageSizeX * 3
            }
        }
    }

    func coeffBufGetp(cb: CoeffBuf, blkX, blkY: s32)->[*] s16
    {
        Debug.assert(blkX < cb.blockNumX)
        Debug.assert(blkY < cb.blockNumY)
        return cast([*] s16) (cb.pdata.buffer + blkX * cb.blockSize + blkY * (cb.blockSize * cb.blockNumX))
    }

    mtd loadNextRow() throw
    {
        var blockMcuX: [MaxComponents] s32

        var rowBlock = 0'u32
        for .mcusPerRow
        {
            var blockMcuOfsX = 0
            var blockMcuOfsY = 0
            for mcuBlock in .blocksPerMcu
            {
                let componentId = .mcuOrg[mcuBlock]

                var p   = .blockSeg[rowBlock]
                let q   = .quant[.compQuant[componentId]].buffer
                var pAC = coeffBufGetp(.acCoeffs[componentId], blockMcuX[componentId] + blockMcuOfsX, .blockMcuY[componentId] + blockMcuOfsY)
                let pDC = coeffBufGetp(.dcCoeffs[componentId], blockMcuX[componentId] + blockMcuOfsX, .blockMcuY[componentId] + blockMcuOfsY)

                p[0] = pDC[0]
                Memory.copy(&p[1], &pAC[1], 63'u64 * #sizeof(s16))

                var i = 63
                while i > 0
                {
                    if p[Zag[cast(u32) i]] do
                        break
                    i -= 1
                }

                while i >= 0
                {
                    if p[Zag[cast(u32) i]] do
                        p[Zag[cast(u32) i]] *= q[cast(u32) i]
                    i -= 1
                }

                rowBlock += 1
                if .compsInScan == 1 do
                    blockMcuX[componentId] += 1
                else
                {
                    blockMcuOfsX += 1
                    if blockMcuOfsX == .compSampH[componentId]
                    {
                        blockMcuOfsX = 0
                        blockMcuOfsY += 1
                        if blockMcuOfsY == .compSampV[componentId]
                        {
                            blockMcuOfsY = 0
                            blockMcuX[componentId] += .compSampH[componentId]
                        }
                    }
                }
            }
        }

        if .compsInScan == 1 do
            .blockMcuY[.compList[0]] += 1
        else
        {
            for componentNum in .compsInScan
            {
                let componentId = .compList[componentNum]
                .blockMcuY[componentId] += .compSampV[componentId]
            }
        }
    }

    mtd restartProcess() throw
    {
        var i = 1536
        while i > 0
        {
            if .getChar() == 0xFF do
                break
            i -= 1
        }

        if i == 0 do
            throw InvalidFormatError{"bad restart marker"}

        var c = 0
        while i > 0
        {
            c = .getChar()
            if c != 0xFF do
                break
            i -= 1
        }

        if i == 0 do
            throw InvalidFormatError{"bad restart marker"}
        if c != (.nextRestartNum + cast(u32) Marker.M_RST0) do
            throw InvalidFormatError{"bad restart marker"}

        Memory.clear(&.lastDcVal, cast(u64) .compsInFrame * #sizeof(u32))

        .eobRun         = 0
        .restartsLeft   = .restartInterval
        .nextRestartNum = (.nextRestartNum + 1) & 7
        .bitsLeft       = 16

        .getBits2(16)
        .getBits2(16)
    }

    mtd huffDecode(ph: *HuffTables)->s32
    {
        var symbol = ph.lookUp[(.bitBuf >> 8) & 0xFF]
        if symbol < 0
        {
            .getBits2(8)
            for
            {
                symbol = ph.tree[~symbol + (1 - .getBits2(1))]
                if symbol >= 0 do
                    break
            }
        }
        else
        {
            .getBits2(cast(s32) ph.codeSize[symbol])
        }

        return symbol
    }

    mtd decodeNextRow() throw
    {
        var rowBlock = 0
        for .mcusPerRow
        {
            if .restartInterval and (.restartsLeft == 0) do
                .restartProcess()

            for mcuBlock in .blocksPerMcu
            {
                let componentId = .mcuOrg[mcuBlock]
                var p           = .blockSeg[rowBlock]
                let q           = .quant[.compQuant[componentId]].buffer

                var s = .huffDecode(&.h[.compDcTab[componentId]])
                if s != 0
                {
                    let r = cast(s32) .getBits2(s)
                    s = r < ExtendTest[s] ? r + ExtendOff[s] : r
                }

                s += .lastDcVal[componentId]
                .lastDcVal[componentId] = s

                p[0] = cast(s16) (s * q[0])
                let prevNumSet = .blockMaxZagSet[rowBlock]
                let ph         = &.h[.compAcTab[componentId]]

                var k = 1
                while k < 64
                {
                    s = .huffDecode(ph)
                    var r = s >> 4
                    s &= 0x0F

                    if s
                    {
                        if r
                        {
                            if (k + r) > 63 do
                                throw InvalidFormatError{"decode error"}
                            if k < prevNumSet
                            {
                                var n  = Math.min(r, prevNumSet - k)
                                var kt = k
                                while n
                                {
                                    p[Zag[kt]] = 0
                                    n -= 1
                                    kt += 1
                                }
                            }

                            k += r
                        }

                        r = cast(s32) .getBits2(s)
                        if s > 15 do
                            throw InvalidFormatError{"decode error"}

                        s         = (r < ExtendTest[s] ? r + ExtendOff[s] : r)
                        p[Zag[k]] = cast(s16) (s * q[k])
                    }
                    else
                    {
                        if r == 15
                        {
                            if (k + 15) > 63 do
                                throw InvalidFormatError{"decode error"}

                            if k < prevNumSet
                            {
                                var n  = Math.min(16'u32, cast(u32) (prevNumSet - k))
                                var kt = k
                                while n
                                {
                                    p[Zag[kt]] = 0
                                    n -= 1
                                    kt += 1
                                }
                            }

                            k += 15
                        }
                        else
                        {
                            break
                        }
                    }

                    k += 1
                }

                if k < prevNumSet
                {
                    var kt = k
                    while kt < prevNumSet
                    {
                        p[Zag[kt]] = 0
                        kt += 1
                    }
                }

                .blockMaxZagSet[rowBlock] = k
                rowBlock += 1
            }

            .restartsLeft -= 1
        }
    }

    func idct(data: [*] s16, buf: [*] u8)
    {
        var pdstPtr = buf
        var dataPtr = data

        for #reverse rowctr in 0 to 7
        {
            // Original code:
            // if (dataPtr[1] | dataPtr[2] | dataPtr[3] | dataPtr[4] | dataPtr[5] | dataPtr[6] | dataPtr[7]) == 0
            let dataPtr64 = cast([*] u64) dataPtr
            let tst       = (dataPtr64[0] & 0xFFFFFFFF_FFFF0000) | dataPtr64[1]
            if tst == 0
            {
                let dcval = cast(s16) (dataPtr[0] << Pass1Bits)
                dataPtr[0] = dcval
                dataPtr[1] = dcval
                dataPtr[2] = dcval
                dataPtr[3] = dcval
                dataPtr[4] = dcval
                dataPtr[5] = dcval
                dataPtr[6] = dcval
                dataPtr[7] = dcval
                dataPtr += 8
                continue
            }

            var z2 = cast(s32) dataPtr[2]
            var z3 = cast(s32) dataPtr[6]

            var z1   = (z2 + z3) * Fix_0_541196100
            var tmp2 = z1 + (z3 * -Fix_1_847759065)
            var tmp3 = z1 + (z2 * Fix_0_765366865)

            let s0 = cast(s32) dataPtr[0]
            let s4 = cast(s32) dataPtr[4]

            var tmp0 = (s0 + s4) << ConstBits
            var tmp1 = (s0 - s4) << ConstBits

            let tmp10 = tmp0 + tmp3
            let tmp13 = tmp0 - tmp3
            let tmp11 = tmp1 + tmp2
            let tmp12 = tmp1 - tmp2

            tmp0 = cast(s32) dataPtr[7]
            tmp1 = cast(s32) dataPtr[5]
            tmp2 = cast(s32) dataPtr[3]
            tmp3 = cast(s32) dataPtr[1]

            z1 = tmp0 + tmp3
            z2 = tmp1 + tmp2
            z3 = tmp0 + tmp2
            var z4 = tmp1 + tmp3
            let z5 = (z3 + z4) * Fix_1_175875602

            tmp0 = (tmp0 * Fix_0_298631336)
            tmp1 = (tmp1 * Fix_2_053119869)
            tmp2 = (tmp2 * Fix_3_072711026)
            tmp3 = (tmp3 * Fix_1_501321110)
            z1   = (z1 * -Fix_0_899976223)
            z2   = (z2 * -Fix_2_562915447)
            z3   = (z3 * -Fix_1_961570560)
            z4   = (z4 * -Fix_0_390180644)

            z3 += z5
            z4 += z5

            tmp0 += z1 + z3
            tmp1 += z2 + z4
            tmp2 += z2 + z3
            tmp3 += z1 + z4

            dataPtr[0] = descale(tmp10 + tmp3, ConstBits - Pass1Bits)
            dataPtr[7] = descale(tmp10 - tmp3, ConstBits - Pass1Bits)
            dataPtr[1] = descale(tmp11 + tmp2, ConstBits - Pass1Bits)
            dataPtr[6] = descale(tmp11 - tmp2, ConstBits - Pass1Bits)
            dataPtr[2] = descale(tmp12 + tmp1, ConstBits - Pass1Bits)
            dataPtr[5] = descale(tmp12 - tmp1, ConstBits - Pass1Bits)
            dataPtr[3] = descale(tmp13 + tmp0, ConstBits - Pass1Bits)
            dataPtr[4] = descale(tmp13 - tmp0, ConstBits - Pass1Bits)

            dataPtr += 8
        }

        dataPtr = data

        for #reverse rowctr in 0 to 7
        {
            if (dataPtr[8 * 1] | dataPtr[8 * 2] | dataPtr[8 * 3] | dataPtr[8 * 4] | dataPtr[8 * 5] | dataPtr[8 * 6] | dataPtr[8 * 7]) == 0
            {
                var dcval = descale(cast(s32) dataPtr[0], Pass1Bits + 3)
                dcval += #wrap 128
                dcval = Math.clamp(dcval, 0's16, 255's16)
                let dcval8 = cast(u8) dcval

                pdstPtr[8 * 0] = dcval8
                pdstPtr[8 * 1] = dcval8
                pdstPtr[8 * 2] = dcval8
                pdstPtr[8 * 3] = dcval8
                pdstPtr[8 * 4] = dcval8
                pdstPtr[8 * 5] = dcval8
                pdstPtr[8 * 6] = dcval8
                pdstPtr[8 * 7] = dcval8

                dataPtr += 1
                pdstPtr += 1
                continue
            }

            var z2 = cast(s32) dataPtr[8 * 2]
            var z3 = cast(s32) dataPtr[8 * 6]

            var z1   = (z2 + z3) * Fix_0_541196100
            var tmp2 = z1 + (z3 * -Fix_1_847759065)
            var tmp3 = z1 + (z2 * Fix_0_765366865)

            let s0 = cast(s32) dataPtr[8 * 0]
            let s4 = cast(s32) dataPtr[8 * 4]

            var tmp0 = (s0 + s4) << ConstBits
            var tmp1 = (s0 - s4) << ConstBits

            let tmp10 = tmp0 + tmp3
            let tmp13 = tmp0 - tmp3
            let tmp11 = tmp1 + tmp2
            let tmp12 = tmp1 - tmp2

            tmp0 = cast(s32) dataPtr[8 * 7]
            tmp1 = cast(s32) dataPtr[8 * 5]
            tmp2 = cast(s32) dataPtr[8 * 3]
            tmp3 = cast(s32) dataPtr[8 * 1]

            z1 = tmp0 + tmp3
            z2 = tmp1 + tmp2
            z3 = tmp0 + tmp2
            var z4 = tmp1 + tmp3
            let z5 = (z3 + z4) * Fix_1_175875602

            tmp0 = (tmp0 * Fix_0_298631336)
            tmp1 = (tmp1 * Fix_2_053119869)
            tmp2 = (tmp2 * Fix_3_072711026)
            tmp3 = (tmp3 * Fix_1_501321110)
            z1   = (z1 * -Fix_0_899976223)
            z2   = (z2 * -Fix_2_562915447)
            z3   = (z3 * -Fix_1_961570560)
            z4   = (z4 * -Fix_0_390180644)

            z3 += z5
            z4 += z5

            tmp0 += z1 + z3
            tmp1 += z2 + z4
            tmp2 += z2 + z3
            tmp3 += z1 + z4

            var i = descale(tmp10 + tmp3, ConstBits + Pass1Bits + 3) + 128's32
            pdstPtr[8 * 0] = clamp(i)

            i              = descale(tmp10 - tmp3, ConstBits + Pass1Bits + 3) + 128's32
            pdstPtr[8 * 7] = clamp(i)

            i              = descale(tmp11 + tmp2, ConstBits + Pass1Bits + 3) + 128's32
            pdstPtr[8 * 1] = clamp(i)

            i              = descale(tmp11 - tmp2, ConstBits + Pass1Bits + 3) + 128's32
            pdstPtr[8 * 6] = clamp(i)

            i              = descale(tmp12 + tmp1, ConstBits + Pass1Bits + 3) + 128's32
            pdstPtr[8 * 2] = clamp(i)

            i              = descale(tmp12 - tmp1, ConstBits + Pass1Bits + 3) + 128's32
            pdstPtr[8 * 5] = clamp(i)

            i              = descale(tmp13 + tmp0, ConstBits + Pass1Bits + 3) + 128's32
            pdstPtr[8 * 3] = clamp(i)

            i              = descale(tmp13 - tmp0, ConstBits + Pass1Bits + 3) + 128's32
            pdstPtr[8 * 4] = clamp(i)

            dataPtr, pdstPtr += 1
        }
    }

    mtd transformRow()
    {
        var psrcPtr = .blockSeg[0]
        var pdstPtr = .pSampleBuf

        for .maxBlocksPerRow
        {
            Memory.copy(.tempBlock, psrcPtr, 64'u64 * #sizeof(s16))
            idct(.tempBlock, pdstPtr)
            psrcPtr += 64
            pdstPtr += 64
        }
    }

    mtd findEoi() throw
    {
        if !.progressiveFlag
        {
            .bitsLeft = 16
            .getBits1(16)
            .getBits1(16)
            .processMarkers()
        }

        .totalBytesRead -= .inBufLeft
    }

    mtd decode(scanLineOfs: **void, scanLineLen: *u32)->s32 throw
    {
        if .totalLinesLeft == 0 do
            return Done

        if .mcuLinesLeft == 0
        {
            if .progressiveFlag do
                .loadNextRow()
            else do
                .decodeNextRow()
            if .totalLinesLeft <= .maxMcuSizeY do
                .findEoi()
            .transformRow()
            .mcuLinesLeft = .maxMcuSizeY
        }

        switch .scanType
        {
        case YH2V2:
            if (.mcuLinesLeft & 1) == 0
            {
                .convertH2V2()
                dref scanLineOfs = .scanLine0.buffer
            }
            else do
                dref scanLineOfs = .scanLine1.buffer

        case YH2V1:
            .convertH2V1()
            dref scanLineOfs = .scanLine0.buffer

        case YH1V2:
            if (.mcuLinesLeft & 1) == 0
            {
                .convertH1V2()
                dref scanLineOfs = .scanLine0.buffer
            }
            else do
                dref scanLineOfs = .scanLine1.buffer

        case YH1V1:
            .convertH1V1()
            dref scanLineOfs = .scanLine0.buffer

        case GrayScale:
            .convertGrayscale()
            dref scanLineOfs = .scanLine0.buffer
        }

        dref scanLineLen = .realDestBytesPerScanLine
        .mcuLinesLeft -= 1
        .totalLinesLeft -= 1

        return Okay
    }

    mtd convertGrayscale()
    {
        let row = .maxMcuSizeY - .mcuLinesLeft
        var d   = .scanLine0.buffer
        var s   = .pSampleBuf + row * 8
        for #reverse i in 1 to .maxMcusPerRow
        {
            d[0] = s[0]
            d[1] = s[1]
            d[2] = s[2]
            d[3] = s[3]
            d[4] = s[4]
            d[5] = s[5]
            d[6] = s[6]
            d[7] = s[7]

            s += 64
            d += 8
        }
    }

    mtd convertH1V1()
    {
        let row = .maxMcuSizeY - .mcuLinesLeft
        var d   = .scanLine0.buffer
        var s   = .pSampleBuf + row * 8
        for #reverse i in 1 to .maxMcusPerRow
        {
            for j in 8
            {
                let y  = cast(s32) s[j]
                let cb = s[64 + j]
                let cr = s[128 + j]

                d[2] = clamp(y + #wrap .crr[cr])
                d[1] = clamp(y + #wrap ((.crg[cr] + .cbg[cb]) >> 16))
                d[0] = clamp(y + #wrap .cbb[cb])
                d += 3
            }

            s += 64 * 3
        }
    }

    mtd convertH1V2()
    {
        let row = .maxMcuSizeY - .mcuLinesLeft
        var d0  = .scanLine0.buffer
        var d1  = .scanLine1.buffer

        var y: [*] u8 = undefined
        if row < 8 do
            y = .pSampleBuf + row * 8
        else do
            y = .pSampleBuf + 64 * 1 + (row & 7) * 8

        var c = .pSampleBuf + 64 * 2 + (row >> 1) * 8
        for #reverse i in 1 to .maxMcusPerRow
        {
            for j in 8
            {
                let cb = c[0 + j]
                let cr = c[64 + j]
                let rc = .crr[cr]
                let gc = ((.crg[cr] + .cbg[cb]) >> 16)
                let bc = .cbb[cb]
                var yy = cast(s32) y[j]

                d0[2] = clamp(yy + rc)
                d0[1] = clamp(yy + gc)
                d0[0] = clamp(yy + bc)

                yy    = y[8 + j]
                d1[2] = clamp(yy + rc)
                d1[1] = clamp(yy + gc)
                d1[0] = clamp(yy + bc)

                d0, d1 += 3
            }

            y, c += 64 * 4
        }
    }

    mtd convertH2V1()
    {
        let row = .maxMcuSizeY - .mcuLinesLeft
        var d0  = .scanLine0.buffer
        var y   = .pSampleBuf + row * 8
        var c   = .pSampleBuf + 2 * 64 + row * 8

        for #reverse i in 1 to .maxMcusPerRow
        {
            for l in 2
            {
                for j in 4
                {
                    let cb = c[0]
                    let cr = c[64]
                    let rc = .crr[cr]
                    let gc = (.crg[cr] + .cbg[cb]) >> 16
                    let bc = .cbb[cb]
                    var yy = cast(s32) y[j << 1]

                    d0[2] = clamp(yy + #wrap rc)
                    d0[1] = clamp(yy + #wrap gc)
                    d0[0] = clamp(yy + #wrap bc)

                    yy    = y[(j << 1) + 1]
                    d0[5] = clamp(yy + #wrap rc)
                    d0[4] = clamp(yy + #wrap gc)
                    d0[3] = clamp(yy + #wrap bc)

                    d0 += 6

                    c += 1
                }

                y += 64
            }

            y += 64 * 4 - 64 * 2
            c += 64 * 4 - 8
        }
    }

    mtd convertH2V2()
    {
        let row = .maxMcuSizeY - .mcuLinesLeft
        var d0  = .scanLine0.buffer
        var d1  = .scanLine1.buffer

        var y: [*] u8 = undefined
        if row < 8 do
            y = .pSampleBuf + row * 8
        else do
            y = .pSampleBuf + 64 * 2 + (row & 7) * 8
        var c = .pSampleBuf + 64 * 4 + (row >> 1) * 8

        for #reverse i in 1 to .maxMcusPerRow
        {
            for l in 2
            {
                for var j = 0; j < 8; j += 2
                {
                    let cb = c[0]
                    let cr = c[64]
                    let rc = .crr[cr]
                    let gc = (.crg[cr] + .cbg[cb]) >> 16
                    let bc = .cbb[cb]
                    var yy = cast(s32) y[j]

                    d0[2] = clamp(yy + #wrap rc)
                    d0[1] = clamp(yy + #wrap gc)
                    d0[0] = clamp(yy + #wrap bc)

                    yy    = y[j + 1]
                    d0[5] = clamp(yy + #wrap rc)
                    d0[4] = clamp(yy + #wrap gc)
                    d0[3] = clamp(yy + #wrap bc)

                    yy    = y[j + 8]
                    d1[2] = clamp(yy + #wrap rc)
                    d1[1] = clamp(yy + #wrap gc)
                    d1[0] = clamp(yy + #wrap bc)

                    yy    = y[j + 8 + 1]
                    d1[5] = clamp(yy + #wrap rc)
                    d1[4] = clamp(yy + #wrap gc)
                    d1[3] = clamp(yy + #wrap bc)

                    d0 += 6
                    d1 += 6

                    c += 1
                }

                y += 64
            }

            y += 64 * 6 - 64 * 2
            c += 64 * 6 - 8
        }
    }

    mtd initFrame() throw
    {
        if .compsInFrame == 1
        {
            .scanType        = GrayScale
            .maxBlocksPerMcu = 1
            .maxMcuSizeX     = 8
            .maxMcuSizeY     = 8
        }
        elif .compsInFrame == 3
        {
            if (.compSampH[1] != 1) or (.compSampV[1] != 1) or (.compSampH[2] != 1) or (.compSampV[2] != 1) do
                throw InvalidFormatError{"invalid samp factors"}

            if (.compSampH[0] == 1) and (.compSampV[0] == 1)
            {
                .scanType        = YH1V1
                .maxBlocksPerMcu = 3
                .maxMcuSizeX     = 8
                .maxMcuSizeY     = 8
            }
            elif (.compSampH[0] == 2) and (.compSampV[0] == 1)
            {
                .scanType        = YH2V1
                .maxBlocksPerMcu = 4
                .maxMcuSizeX     = 16
                .maxMcuSizeY     = 8
            }
            elif (.compSampH[0] == 1) and (.compSampV[0] == 2)
            {
                .scanType        = YH1V2
                .maxBlocksPerMcu = 4
                .maxMcuSizeX     = 8
                .maxMcuSizeY     = 16
            }
            elif (.compSampH[0] == 2) and (.compSampV[0] == 2)
            {
                .scanType        = YH2V2
                .maxBlocksPerMcu = 6
                .maxMcuSizeX     = 16
                .maxMcuSizeY     = 16
            }
            else do
                throw InvalidFormatError{"invalid samp factors"}
        }
        else do
            throw UnsupportedFormatError{"unsupported color space"}

        .maxMcusPerRow = (.imageSizeX + (.maxMcuSizeX - 1)) / .maxMcuSizeX
        .maxMcusPerCol = (.imageSizeY + (.maxMcuSizeY - 1)) / .maxMcuSizeY

        if .scanType == GrayScale do
            .destBytesPerPixel = 1
        else do
            .destBytesPerPixel = 4

        .destBytesPerScanLine     = ((.imageSizeX + 15) & 0xFFF0) * .destBytesPerPixel
        .realDestBytesPerScanLine = (.imageSizeX * .destBytesPerPixel)

        .scanLine0.resize(cast(u64) .destBytesPerScanLine + 8)
        Memory.clear(.scanLine0.buffer, cast(u64) .destBytesPerScanLine)

        .scanLine1.resize(cast(u64) .destBytesPerScanLine + 8)
        Memory.clear(.scanLine1.buffer, cast(u64) .destBytesPerScanLine)

        .maxBlocksPerRow = .maxMcusPerRow * .maxBlocksPerMcu
        if .maxBlocksPerRow > MaxBlocksPerRow do
            throw InvalidFormatError{"assertion error"}

        .quantBuf.resize(cast(u64) .maxBlocksPerRow * 64 * #sizeof(s16) + 8)
        for i in .maxBlocksPerRow
        {
            .blockSeg[i]       = cast([*] s16) (.quantBuf.buffer + i * 64 * #sizeof(s16))
            .blockMaxZagSet[i] = 64
        }

        .sampleBuf.resize(cast(u64) .maxBlocksPerRow * 64 + 8)
        .pSampleBuf = .sampleBuf.buffer

        .totalLinesLeft = .imageSizeY
        .mcuLinesLeft   = 0
        .createLookUps()
    }

    mtd createLookUps()
    {
        var fix: [4] s32 = undefined
        fix[0] = cast(s32) ((1.40200 / 2) * (1 << 16) + 0.5)
        fix[1] = cast(s32) ((1.77200 / 2) * (1 << 16) + 0.5)
        fix[2] = cast(s32) -((0.71414 / 2) * (1 << 16) + 0.5)
        fix[3] = cast(s32) -((0.34414 / 2) * (1 << 16) + 0.5)

        for i in 256
        {
            let k = (i * 2) - 256
            .crr[i] = (fix[0] * k + (1 << (16 - 1))) >> 16
            .cbb[i] = (fix[1] * k + (1 << (16 - 1))) >> 16
            .crg[i] = (fix[2] * k)
            .cbg[i] = (fix[3] * k + (1 << (16 - 1)))
        }
    }

    mtd initScan()->bool throw
    {
        let res = .locateSosMarker()
        if !res do
            return false

        .calcMcuBlockOrder()
        .checkHuffTables()
        .checkQuantTables()

        Memory.clear(.lastDcVal, cast(u64) .compsInFrame * #sizeof(u32))

        .eobRun = 0
        if .restartInterval
        {
            .restartsLeft   = .restartInterval
            .nextRestartNum = 0
        }

        .fixInBuffer()
        return true
    }

    mtd stuffChar(q: u8)
    {
        .inBufOfs -= 1
        dref .inBufOfs = q
        .inBufLeft += 1
    }

    mtd fixInBuffer()
    {
        if .bitsLeft == 16 do
            .stuffChar(cast(u8) ((.bitBuf >> 16) & 0xFF))
        if .bitsLeft >= 8 do
            .stuffChar(cast(u8) ((.bitBuf >> 24) & 0xFF))

        .stuffChar(cast(u8) (.bitBuf & 0xFF))
        .stuffChar(cast(u8) ((.bitBuf >> 8) & 0xFF))
        .bitsLeft = 16

        .getBits2(16)
        .getBits2(16)
    }

    mtd checkQuantTables() throw
    {
        for i in .compsInScan
        {
            if .quant[.compQuant[.compList[i]]].count == 0 do
                throw InvalidFormatError{"undefined quant table"}
        }
    }

    mtd makeHuffTable(idx: s32, hs: *HuffTables)
    {
        var huffSize: [257] u8
        var huffCode: [257] u32

        var p = 0
        for l in 1 to 16
        {
            for var i = 1; i <= .huffNum[idx].buffer[l]; i += 1
            {
                huffSize[p] = cast(u8) l
                p += 1
            }
        }

        huffSize[p] = 0
        let lastp = p
        var cde   = 0
        var si    = huffSize[0]
        p = 0

        while huffSize[p]
        {
            while huffSize[p] == si
            {
                huffCode[p] = cde
                p, cde += 1
            }

            cde <<= 1
            si += 1
        }

        Memory.clear(hs.lookUp, #sizeof(hs.lookUp))
        Memory.clear(hs.tree, #sizeof(hs.tree))
        Memory.clear(hs.codeSize, #sizeof(hs.codeSize))

        var nextFreeEntry = -1
        p = 0
        while p < lastp
        {
            let i = .huffVal[idx].buffer[p]
            cde = huffCode[p]
            let codeSize = huffSize[p]
            hs.codeSize[i] = cast(u8) codeSize
            if codeSize <= 8
            {
                cde <<= (8 - codeSize)
                for var l = 1 << (8 - codeSize); l > 0; l -= 1
                {
                    hs.lookUp[cde] = i
                    cde += 1
                }
            }
            else
            {
                let subtree      = (cde >> (codeSize - 8)) & 0xFF
                var currentEntry = hs.lookUp[subtree]
                if currentEntry == 0
                {
                    hs.lookUp[subtree] = nextFreeEntry
                    currentEntry       = nextFreeEntry
                    nextFreeEntry -= 2
                }

                cde <<= (16 - (codeSize - 8))
                for var l = codeSize; l > 9; l -= 1
                {
                    if (cde & 0x8000) == 0 do
                        currentEntry -= 1

                    if hs.tree[-currentEntry - 1] == 0
                    {
                        hs.tree[-currentEntry - 1] = nextFreeEntry
                        currentEntry               = nextFreeEntry
                        nextFreeEntry -= 2
                    }
                    else do
                        currentEntry = hs.tree[-currentEntry - 1]

                    cde <<= 1
                }

                if (cde & 0x8000) == 0 do
                    currentEntry -= 1
                hs.tree[-currentEntry - 1] = i
            }

            p += 1
        }
    }

    mtd checkHuffTables() throw
    {
        for i in .compsInScan
        {
            if (.spectralStart == 0) and (.huffNum[.compDcTab[.compList[i]]].count == 0) do
                throw InvalidFormatError{"undefined huff table"}

            if (.spectralEnd > 0) and (.huffNum[.compAcTab[.compList[i]]].count == 0) do
                throw InvalidFormatError{"undefined huff table"}
        }

        for i in MaxHuffTables
        {
            if .huffNum[i].count
            {
                .makeHuffTable(i, &.h[i])
            }
        }

        for i in .blocksPerMcu
        {
            .dcHuffSeg[i] = &.h[.compDcTab[.mcuOrg[i]]]
            .acHuffSeg[i] = &.h[.compAcTab[.mcuOrg[i]]]
            .component[i] = cast(*u32) &.lastDcVal[.mcuOrg[i]]
        }
    }

    mtd calcMcuBlockOrder()
    {
        var maxSampH = 1
        var maxSampV = 1

        for cid in .compsInFrame
        {
            maxSampH = Math.max(maxSampH, .compSampH[cid])
            maxSampV = Math.max(maxSampV, .compSampV[cid])
        }

        for cid in .compsInFrame
        {
            .compBlocksH[cid] = ((((.imageSizeX * .compSampH[cid]) + (maxSampH - 1)) / maxSampH) + 7) / 8
            .compBlocksV[cid] = ((((.imageSizeY * .compSampV[cid]) + (maxSampV - 1)) / maxSampV) + 7) / 8
        }

        if .compsInScan == 1
        {
            .mcusPerRow = .compBlocksH[.compList[0]]
            .mcusPerCol = .compBlocksV[.compList[0]]
        }
        else
        {
            .mcusPerRow = (((.imageSizeX + 7) / 8) + (maxSampH - 1)) / maxSampH
            .mcusPerCol = (((.imageSizeY + 7) / 8) + (maxSampV - 1)) / maxSampV
        }

        if .compsInScan == 1
        {
            .mcuOrg[0]    = .compList[0]
            .blocksPerMcu = 1
        }
        else
        {
            .blocksPerMcu = 0
            for cNum in .compsInScan
            {
                let cid        = .compList[cNum]
                var num_blocks = .compSampH[cid] * .compSampV[cid]
                while num_blocks
                {
                    .mcuOrg[.blocksPerMcu] = cid
                    .blocksPerMcu += 1
                    num_blocks -= 1
                }
            }
        }
    }

    func coeffBufOpen(blkNumX, blkNumY, blkLenX, blkLenY: s32)->CoeffBuf
    {
        with var result: retval
        {
            .blockNumX = blkNumX
            .blockNumY = blkNumY
            .blockLenX = blkLenX
            .blockLenY = blkLenY
            .blockSize = (blkLenX * blkLenY) * #sizeof(s16)
            .pdata.resize(cast(u64) .blockSize * blkNumX * blkNumY)
        }

        return result
    }

    mtd decodeBlockDcFirst(cmpId: u32, blkX, blkY: s32) throw
    {
        var p = coeffBufGetp(.dcCoeffs[cmpId], blkX, blkY)
        var s = .huffDecode(&.h[.compDcTab[cmpId]])
        if s != 0
        {
            let r = .getBits2(s)
            s = (r < ExtendTest[s] ? r + ExtendOff[s] : cast(s32) r)
        }

        s += .lastDcVal[cmpId]
        .lastDcVal[cmpId] = s
        p[0]              = cast(s16) (s << .successiveLow)
    }

    mtd decodeBlockDcRefine(cmpId: u32, blkX, blkY: s32) throw
    {
        if .getBits2(1)
        {
            var p = coeffBufGetp(.dcCoeffs[cmpId], blkX, blkY)
            p[0] |= (1's16 << .successiveLow)
        }
    }

    mtd decodeBlockAcFirst(cmpId: u32, blkX, blkY: s32) throw
    {
        if .eobRun
        {
            .eobRun -= 1
            return
        }

        var p = coeffBufGetp(.acCoeffs[cmpId], blkX, blkY)

        for var k = .spectralStart; k <= .spectralEnd; k += 1
        {
            var s = .huffDecode(&.h[.compAcTab[cmpId]])
            var r = s >> 4
            s &= 0x0F

            if s
            {
                k += r
                if k > 63 do
                    throw InvalidFormatError{"decode error"}
                r = .getBits2(s)
                if s > 15 do
                    throw InvalidFormatError{"decode error"}

                s         = (r < ExtendTest[s] ? r + ExtendOff[s] : r)
                p[Zag[k]] = cast(s16) (s << .successiveLow)
            }
            else
            {
                if r == 15
                {
                    k += 15
                    if k > 63 do
                        throw InvalidFormatError{"decode error"}
                }
                else
                {
                    .eobRun = 1 << r
                    if r do
                        .eobRun += .getBits2(r)
                    .eobRun -= 1
                    break
                }
            }
        }
    }

    mtd decodeBlockAcRefine(cmpId: u32, blkX, blkY: s32) throw
    {
        let p1 = 1 << .successiveLow
        let m1 = -1 << .successiveLow
        var p  = coeffBufGetp(.acCoeffs[cmpId], blkX, blkY)
        var k  = .spectralStart
        if .eobRun == 0
        {
            if .spectralEnd >= 64 do
                throw InvalidFormatError{"decode error"}

            while k <= .spectralEnd
            {
                var s = .huffDecode(&.h[.compAcTab[cmpId]])
                var r = s >> 4
                s &= 15

                if s
                {
                    if s != 1 do
                        throw InvalidFormatError{"decode error"}
                    if .getBits2(1) do
                        s = p1
                    else do
                        s = m1
                }
                else
                {
                    if r != 15
                    {
                        .eobRun = 1 << r
                        if r do
                            .eobRun += .getBits2(r)
                        break
                    }
                }

                for
                {
                    var thisCoef = p + Zag[k]
                    if dref thisCoef != 0
                    {
                        if .getBits2(1)
                        {
                            if (dref thisCoef & cast(s16) p1) == 0
                            {
                                if dref thisCoef >= 0 do
                                    dref thisCoef += cast(s16) p1
                                else do
                                    dref thisCoef += cast(s16) m1
                            }
                        }
                    }
                    else
                    {
                        r -= 1
                        if r < 0 do
                            break
                    }

                    k += 1
                    if k > .spectralEnd do
                        break
                }

                if s and (k < 64)
                {
                    p[Zag[k]] = cast(s16) s
                }

                k += 1
            }
        }

        if .eobRun > 0
        {
            if .spectralEnd >= 64 do
                throw InvalidFormatError{"decode error"}
            while k <= .spectralEnd
            {
                var thisCoef = p + Zag[k]
                if dref thisCoef != 0
                {
                    if .getBits2(1)
                    {
                        if (dref thisCoef & cast(s16) p1) == 0
                        {
                            if dref thisCoef >= 0 do
                                dref thisCoef += cast(s16) p1
                            else do
                                dref thisCoef += cast(s16) m1
                        }
                    }
                }

                k += 1
            }

            .eobRun -= 1
        }
    }

    mtd decodeScan(fn: func(*Decoder, u32, s32, s32) throw) throw
    {
        var blockMcutX: [MaxComponents] s32
        var blockMcutY: [MaxComponents] s32

        for .mcusPerCol
        {
            Memory.clear(blockMcutX, #sizeof(blockMcutX))
            for .mcusPerRow
            {
                var blockMcuOfsX = 0
                var blockMcuOfsY = 0

                if .restartInterval and (.restartsLeft == 0) do
                    .restartProcess()

                for mcuBlock in .blocksPerMcu
                {
                    let componentId = .mcuOrg[mcuBlock]
                    fn(me, componentId, blockMcutX[componentId] + blockMcuOfsX, blockMcutY[componentId] + blockMcuOfsY)

                    if .compsInScan == 1 do
                        blockMcutX[componentId] += 1
                    else
                    {
                        blockMcuOfsX += 1
                        if blockMcuOfsX == .compSampH[componentId]
                        {
                            blockMcuOfsX = 0
                            blockMcuOfsY += 1
                            if blockMcuOfsY == .compSampV[componentId]
                            {
                                blockMcuOfsY = 0
                                blockMcutX[componentId] += .compSampH[componentId]
                            }
                        }
                    }
                }

                .restartsLeft -= 1
            }

            if .compsInScan == 1 do
                blockMcutY[.compList[0]] += 1
            else
            {
                for n in .compsInScan
                {
                    let componentId = .compList[n]
                    blockMcutY[componentId] += .compSampV[componentId]
                }
            }
        }
    }

    mtd initProgressive() throw
    {
        if .compsInFrame == 4 do
            throw UnsupportedFormatError{"unsupported color space"}

        for i in .compsInFrame
        {
            .dcCoeffs[i] = coeffBufOpen(.maxMcusPerRow * .compSampH[i], .maxMcusPerCol * .compSampV[i], 1, 1)
            .acCoeffs[i] = coeffBufOpen(.maxMcusPerRow * .compSampH[i], .maxMcusPerCol * .compSampV[i], 8, 8)
        }

        for
        {
            let res = .initScan()
            if !res do
                break

            let dcOnlyScan     = (.spectralStart == 0)
            let refinementScan = (.successiveHigh != 0)

            if (.spectralStart > .spectralEnd) or (.spectralEnd > 63) do
                throw InvalidFormatError{"bad sos spectral"}

            if dcOnlyScan
            {
                if .spectralEnd do
                    throw InvalidFormatError{"bad sos spectral"}
            }
            elif .compsInScan != 1
            {
                throw InvalidFormatError{"bad sos spectral"}
            }

            if refinementScan and (.successiveLow != .successiveHigh - 1) do
                throw InvalidFormatError{"bad sos successive"}

            var decodeBlockFunc: func(*Decoder, u32, s32, s32) throw
            if dcOnlyScan
            {
                if refinementScan do
                    decodeBlockFunc = &decodeBlockDcRefine
                else do
                    decodeBlockFunc = &decodeBlockDcFirst
            }
            else
            {
                if refinementScan do
                    decodeBlockFunc = &decodeBlockAcRefine
                else do
                    decodeBlockFunc = &decodeBlockAcFirst
            }

            .decodeScan(decodeBlockFunc)

            .bitsLeft = 16

            .getBits1(16)
            .getBits1(16)
        }

        .compsInScan = .compsInFrame
        for i in .compsInFrame do
            .compList[i] = i
        .calcMcuBlockOrder()
    }

    mtd initSequential() throw
    {
        let res = .initScan()
        if !res do
            throw InvalidFormatError{"unexpected marker"}
    }

    #[Discardable]
    mtd getBits1(numBits: s32)->u32
    {
        let i = .bitBuf >> cast(u32) (16 - numBits) & ((1'u32 << cast(u32) numBits) - 1)
        .bitsLeft -= numBits
        if .bitsLeft <= 0
        {
            .bitBuf = rol(.bitBuf, cast(u32) (numBits + .bitsLeft))
            let c1 = .getChar()
            let c2 = .getChar()

            .bitBuf = (.bitBuf & 0xFFFF) | (cast(u32) c1 << 24) | (cast(u32) c2 << 16)
            .bitBuf = rol(.bitBuf, cast(u32) -.bitsLeft)
            .bitsLeft += 16
        }
        else do
            .bitBuf = rol(.bitBuf, cast(u32) numBits)

        return i
    }

    #[Discardable]
    mtd getBits2(numBits: s32)->u32
    {
        let i = (.bitBuf >> cast(u32) (16 - numBits)) & ((1'u32 << cast(u32) numBits) - 1)
        .bitsLeft -= numBits
        if .bitsLeft <= 0
        {
            .bitBuf = rol(.bitBuf, cast(u32) (numBits + .bitsLeft))
            let c1 = .getOctet()
            let c2 = .getOctet()

            .bitBuf = (.bitBuf & 0xFFFF) | (cast(u32) c1 << 24) | (cast(u32) c2 << 16)
            .bitBuf = rol(.bitBuf, cast(u32) -.bitsLeft)
            .bitsLeft += 16
        }
        else do
            .bitBuf = rol(.bitBuf, cast(u32) numBits)

        return i
    }

    mtd getOctet()->u8
    {
        var paddingFlag: bool
        var c            = .getChar(&paddingFlag)
        if c == 0xFF
        {
            if paddingFlag do
                return 0xFF

            c = .getChar(&paddingFlag)
            if paddingFlag
            {
                .stuffChar(0xFF)
                return 0xFF
            }

            if c == 0x00 do
                return 0xFF

            .stuffChar(cast(u8) c)
            .stuffChar(0xFF)
            return 0xFF
        }

        return cast(u8) c
    }

    #[Swag.Overload]
    mtd getChar(paddingFlag: *bool)->u32
    {
        if !.inBufLeft
        {
            .prepareInBuffer()
            if !.inBufLeft
            {
                dref paddingFlag = true
                let t = .temFlag
                .temFlag ^= 1
                if t do
                    return 0xD9
                return 0xFF
            }
        }

        dref paddingFlag = false
        let c = dref .inBufOfs
        .inBufOfs += 1
        .inBufLeft -= 1
        return c
    }

    #[Swag.Overload]
    mtd getChar()->u32
    {
        if !.inBufLeft
        {
            .prepareInBuffer()
            if !.inBufLeft
            {
                let t = .temFlag
                .temFlag ^= 1
                if t do
                    return 0xD9
                return 0xFF
            }
        }

        let c = .inBufOfs[0]
        .inBufOfs += 1
        .inBufLeft -= 1
        return c
    }

    mtd nextMarker()->Marker
    {
        var c: u32

        var bytes = 0
        while !c
        {
            while c != 0xFF
            {
                bytes += 1
                c = .getBits1(8)
            }

            while c == 0xFF
            {
                c = .getBits1(8)
            }
        }

        return cast(Marker) c
    }

    #[Discardable]
    mtd processMarkers()->Marker throw
    {
        for
        {
            let c = .nextMarker()
            switch c
            {
            case M_SOF0, M_SOF1, M_SOF2, M_SOF3, M_SOF5, M_SOF6, M_SOF7, M_SOF9, M_SOF10, M_SOF11, M_SOF13, M_SOF14, M_SOF15, M_SOI, M_EOI, M_SOS:
                return c
            case M_JPG, M_RST0, M_RST1, M_RST2, M_RST3, M_RST4, M_RST5, M_RST6, M_RST7, M_TEM:
                throw InvalidFormatError{"unexpected marker"}
            case M_DAC:
                throw UnsupportedFormatError{"no arithmetic support"}
            case M_DHT:
                .readDhtMarker()
            case M_DQT:
                .readDqtMarker()
            case M_DRI:
                .readDriMarker()
            default:
                if cast(u32) c & cast(u32) M_APP0 == cast(u32) M_APP0 do
                    .readAppMarker(c)
                else do
                    .readVariableMarker(c)
            }
        }

        return Marker.M_ERROR
    }

    mtd readVariableMarker(_marker: Marker) throw
    {
        var left = .getBits1(16)
        if left < 2 do
            throw InvalidFormatError{"bad variable marker"}
        left -= 2

        @assert(!.acceptMetaDatas)
        while left
        {
            .getBits1(8)
            left -= 1
        }
    }

    mtd readDhtMarker() throw
    {
        var huffNumt: [17] u8
        var huffValt: [256] u8

        var left = .getBits1(16)
        if left < 2 do
            throw InvalidFormatError{"bad dht marker"}
        left -= 2

        while left
        {
            var index = .getBits1(8)
            var count = 0
            huffNumt[0] = 0
            for i in 1 to 16
            {
                huffNumt[i] = cast(u8) .getBits1(8)
                count += huffNumt[i]
            }

            if count > 255 do
                throw InvalidFormatError{"bad dht counts"}
            for i in count
            {
                huffValt[i] = cast(u8) .getBits1(8)
            }

            let i = 1 + 16 + count
            if left < i do
                throw InvalidFormatError{"bad dht marker"}

            left -= i
            if (index & 0x10) > 0x10 do
                throw InvalidFormatError{"bad dht index"}

            index = (index & 0x0F) + ((index & 0x10) >> 4) * (MaxHuffTables >> 1)
            if index >= MaxHuffTables do
                throw InvalidFormatError{"bad dht index"}

            .huffNum[index].resize(17)
            .huffVal[index].resize(256)
            Memory.copy(.huffNum[index].buffer, huffNumt, 17)
            Memory.copy(.huffVal[index].buffer, huffValt, 256)
        }
    }

    mtd readDqtMarker() throw
    {
        var left = .getBits1(16)
        if left < 2 do
            throw InvalidFormatError{"bad dqt marker"}
        left -= 2

        while left
        {
            var n    = .getBits1(8)
            let prec = n >> 4
            n &= 0x0F
            if n >= MaxQuantTables do
                throw InvalidFormatError{"bad dqt table"}

            .quant[n].resize(64)
            for i in 64
            {
                var temp = .getBits1(8)
                if prec do
                    temp = (temp << 8) + .getBits1(8)
                .quant[n].buffer[i] = cast(s16) temp
            }

            var i = 64 + 1
            if prec do
                i += 64
            if left < i do
                throw InvalidFormatError{"bad dqt length"}
            left -= i
        }
    }

    mtd readDriMarker() throw
    {
        if .getBits1(16) != 4 do
            throw InvalidFormatError{"bad dri length"}
        .restartInterval = .getBits1(16)
    }

    mtd readSofMarker() throw
    {
        let left = .getBits1(16)
        if .getBits1(8) != 8 do
            throw InvalidFormatError{"bad precision"}

        .imageSizeY = .getBits1(16)
        .imageSizeX = .getBits1(16)

        if (.imageSizeY < 1) or (.imageSizeY > MaxHeight) do
            throw InvalidFormatError{"bad height"}
        if (.imageSizeX < 1) or (.imageSizeX > MaxWidth) do
            throw InvalidFormatError{"bad width"}

        .compsInFrame = .getBits1(8)
        if .compsInFrame > MaxComponents do
            throw InvalidFormatError{"too many components"}
        if left != (.compsInFrame * 3 + 8) do
            throw InvalidFormatError{"bad sof length"}

        for i in .compsInFrame
        {
            .compIdent[i] = .getBits1(8)
            .compSampH[i] = .getBits1(4)
            .compSampV[i] = .getBits1(4)
            .compQuant[i] = .getBits1(8)
        }
    }

    mtd readAppMarker(marker: Marker) throw
    {
        var left = .getBits1(16)
        if left < 2 do
            throw InvalidFormatError{"Bad app marker"}
        left -= 2

        if !.decOptions.decodeMetaDatas
        {
            while left
            {
                .getBits1(8)
                left -= 1
            }
        }
        else
        {
            var meta: ImageMetaData
            meta.sig = cast(u32) marker - cast(u32) Marker.M_APP0

            while left
            {
                let c = cast(u8) .getBits1(8)
                left -= 1
                if c == 0 do
                    break
                meta.tag.add(c)
            }

            while left
            {
                let c = cast(u8) .getBits1(8)
                meta.value.add(c)
                left -= 1
            }

            .metaDatas.add(meta)
        }
    }

    mtd readSosMarker() throw
    {
        var left = .getBits1(16)
        let n    = .getBits1(8)
        .compsInScan = n
        left -= 3

        if (left != (n * 2 + 3)) or (n < 1) or (n > MaxCompsInScan) do
            throw InvalidFormatError{"bad sos length"}

        for i in n
        {
            let cc = .getBits1(8)
            let c  = .getBits1(8)
            left -= 2

            var ci = 0
            while ci < .compsInFrame
            {
                if cc == .compIdent[ci] do
                    break
                ci += 1
            }

            if ci >= .compsInFrame do
                throw InvalidFormatError{"bad sos comp id"}

            .compList[i]   = ci
            .compDcTab[ci] = (c >> 4) & 15
            .compAcTab[ci] = (c & 15) + (MaxHuffTables >> 1)
        }

        .spectralStart  = .getBits1(8)
        .spectralEnd    = .getBits1(8)
        .successiveHigh = .getBits1(4)
        .successiveLow  = .getBits1(4)

        if !.progressiveFlag
        {
            .spectralStart = 0
            .spectralEnd   = 63
        }

        left -= 3
        while left
        {
            .getBits1(8)
            left -= 1
        }
    }

    mtd locateSosMarker()->bool throw
    {
        let c = .processMarkers()
        if c == .M_EOI do
            return false
        elif c != .M_SOS do
            throw InvalidFormatError{"unexpected marker"}
        .readSosMarker()
        return true
    }

    mtd locateSoiMarker() throw
    {
        var lastRune = .getBits1(8)
        var thisChar = .getBits1(8)
        if (lastRune == 0xFF) and (thisChar == Marker.M_SOI) do
            return
        var bytesLeft = 512
        for
        {
            bytesLeft -= 1
            if bytesLeft == 0 do
                throw InvalidFormatError{"not a jpeg"}
            lastRune = thisChar
            thisChar = .getBits1(8)
            if (lastRune == 0xFF) and (thisChar == Marker.M_SOI) do
                break
        }

        thisChar = (.bitBuf >> 8) & 0xFF
        if thisChar != 0xFF do
            throw InvalidFormatError{"not a jpeg"}
    }

    mtd locateSofMarker() throw
    {
        .locateSoiMarker()
        let c = .processMarkers()
        switch c
        {
        case M_SOF2:
            .progressiveFlag = true
            try .readSofMarker()
        case M_SOF0, M_SOF1:
            .readSofMarker()
        case M_SOF9:
            throw UnsupportedFormatError{"no arithmetic support"}
        default:
            throw UnsupportedFormatError{"unsupported marker"}
        }
    }

    func memset16(p: [*] void, c: u16, n: u32)
    {
        var ps = cast([*] u16) p
        for n
        {
            dref ps = c
            ps += 1
        }
    }

    mtd prepareInBuffer()
    {
        .inBufLeft = 0
        .inBufOfs  = .inBuf
        if .eofReached do
            return

        for
        {
            var size = InBufSize - .inBufLeft
            if size + .streamSeek > .streamLen
            {
                size        = .streamLen - .streamSeek
                .eofReached = 1
            }

            let bytesRead = size
            Memory.copy(&.inBuf[0] + .inBufLeft, .stream + .streamSeek, cast(u64) size)
            .streamSeek += size
            .inBufLeft += bytesRead
            if .inBufLeft >= InBufSize or .eofReached do
                break
        }

        .totalBytesRead += .inBufLeft
        memset16(.inBufOfs + .inBufLeft, 0xD9FF, 64)
    }
}

public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        let ext = Path.getExtensionLowerCase(fileName)
        return ext == ".jpg" or ext == ".jpeg"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        var result: retval
        .decOptions = options

        .init(buffer)
        .start()

        // Creates image
        result.init(.imageSizeX, .imageSizeY, PixelFormat.BGR8)
        if !options.decodePixels do
            return result
        result.allocPixels()

        .doit(result.pixels.buffer)
        result.metaDatas = #move .metaDatas

        return result
    }
}
