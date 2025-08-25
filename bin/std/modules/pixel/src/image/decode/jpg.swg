#global marked
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
    stream:                       const ^u8
    inBufOfs:                     ^u8
    pSampleBuf:                   ^u8

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
    blockSeg:                     [MaxBlocksPerRow] ^s16
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
        me.stream    = @dataof(buffer)
        me.streamLen = cast(s32) @countof(buffer)

        me.inBufOfs = me.inBuf
        me.prepareInBuffer()
        me.bitsLeft = 16
        me.getBits1(16)
        me.getBits1(16)
        for i in MaxBlocksPerRow:
            me.blockMaxZagSet[i] = 64

        me.locateSofMarker()
    }

    mtd start() throw
    {
        me.initFrame()
        if me.progressiveFlag:
            me.initProgressive()
        else:
            me.initSequential()
    }

    mtd doit(buf: ^u8) throw
    {
        var scanLineOfs: *void
        var scanLineLen: u32

        // Gray scale (1BPP)
        if me.compsInFrame == 1
        {
            var pdest = buf
            pdest += (me.imageSizeY - 1) * me.imageSizeX * 3
            for
            {
                if me.decode(&scanLineOfs, &scanLineLen):
                    break

                var pdest1 = pdest
                var psrc   = cast(^u8) scanLineOfs
                for me.imageSizeX
                {
                    pdest1[0] = dref psrc
                    pdest1[1] = dref psrc
                    pdest1[2] = dref psrc
                    pdest1 += 3
                    psrc += 1
                }

                pdest -= me.imageSizeX * 3
            }
        }

        // 3 BPP
        else
        {
            var pdest = buf
            pdest += (me.imageSizeY - 1) * me.imageSizeX * 3
            for
            {
                if me.decode(&scanLineOfs, &scanLineLen):
                    break
                Memory.copy(pdest, scanLineOfs, cast(u64) me.imageSizeX * 3)
                pdest -= me.imageSizeX * 3
            }
        }
    }

    func coeffBufGetp(cb: CoeffBuf, blkX, blkY: s32)->^s16
    {
        Debug.assert(blkX < cb.blockNumX)
        Debug.assert(blkY < cb.blockNumY)
        return cast(^s16) (cb.pdata.buffer + blkX * cb.blockSize + blkY * (cb.blockSize * cb.blockNumX))
    }

    mtd loadNextRow() throw
    {
        var blockMcuX: [MaxComponents] s32

        var rowBlock = 0'u32
        for me.mcusPerRow
        {
            var blockMcuOfsX = 0
            var blockMcuOfsY = 0
            for mcuBlock in me.blocksPerMcu
            {
                let componentId = me.mcuOrg[mcuBlock]

                var p   = me.blockSeg[rowBlock]
                let q   = me.quant[me.compQuant[componentId]].buffer
                var pAC = coeffBufGetp(me.acCoeffs[componentId], blockMcuX[componentId] + blockMcuOfsX, me.blockMcuY[componentId] + blockMcuOfsY)
                let pDC = coeffBufGetp(me.dcCoeffs[componentId], blockMcuX[componentId] + blockMcuOfsX, me.blockMcuY[componentId] + blockMcuOfsY)

                p[0] = pDC[0]
                Memory.copy(&p[1], &pAC[1], 63'u64 * #sizeof(s16))

                var i = 63
                while i > 0
                {
                    if p[Zag[cast(u32) i]]:
                        break
                    i -= 1
                }

                while i >= 0
                {
                    if p[Zag[cast(u32) i]]:
                        p[Zag[cast(u32) i]] *= q[cast(u32) i]
                    i -= 1
                }

                rowBlock += 1
                if me.compsInScan == 1:
                    blockMcuX[componentId] += 1
                else
                {
                    blockMcuOfsX += 1
                    if blockMcuOfsX == me.compSampH[componentId]
                    {
                        blockMcuOfsX = 0
                        blockMcuOfsY += 1
                        if blockMcuOfsY == me.compSampV[componentId]
                        {
                            blockMcuOfsY = 0
                            blockMcuX[componentId] += me.compSampH[componentId]
                        }
                    }
                }
            }
        }

        if me.compsInScan == 1:
            me.blockMcuY[me.compList[0]] += 1
        else
        {
            for componentNum in me.compsInScan
            {
                let componentId = me.compList[componentNum]
                me.blockMcuY[componentId] += me.compSampV[componentId]
            }
        }
    }

    mtd restartProcess() throw
    {
        var i = 1536
        while i > 0
        {
            if me.getChar() == 0xFF:
                break
            i -= 1
        }

        if i == 0:
            throw InvalidFormatError{"bad restart marker"}

        var c = 0
        while i > 0
        {
            c = me.getChar()
            if c != 0xFF:
                break
            i -= 1
        }

        if i == 0:
            throw InvalidFormatError{"bad restart marker"}
        if c != (me.nextRestartNum + cast(u32) Marker.M_RST0):
            throw InvalidFormatError{"bad restart marker"}

        Memory.clear(&me.lastDcVal, cast(u64) me.compsInFrame * #sizeof(u32))

        me.eobRun         = 0
        me.restartsLeft   = me.restartInterval
        me.nextRestartNum = (me.nextRestartNum + 1) & 7
        me.bitsLeft       = 16

        me.getBits2(16)
        me.getBits2(16)
    }

    mtd huffDecode(ph: *HuffTables)->s32
    {
        var symbol = ph.lookUp[(me.bitBuf >> 8) & 0xFF]
        if symbol < 0
        {
            me.getBits2(8)
            for
            {
                symbol = ph.tree[~symbol + (1 - me.getBits2(1))]
                if symbol >= 0:
                    break
            }
        }
        else
        {
            me.getBits2(cast(s32) ph.codeSize[symbol])
        }

        return symbol
    }

    mtd decodeNextRow() throw
    {
        var rowBlock = 0
        for me.mcusPerRow
        {
            if me.restartInterval and (me.restartsLeft == 0):
                me.restartProcess()

            for mcuBlock in me.blocksPerMcu
            {
                let componentId = me.mcuOrg[mcuBlock]
                var p           = me.blockSeg[rowBlock]
                let q           = me.quant[me.compQuant[componentId]].buffer

                var s = me.huffDecode(&me.h[me.compDcTab[componentId]])
                if s != 0
                {
                    let r = cast(s32) me.getBits2(s)
                    s = r < ExtendTest[s] ? r + ExtendOff[s] : r
                }

                s += me.lastDcVal[componentId]
                me.lastDcVal[componentId] = s

                p[0] = cast(s16) (s * q[0])
                let prevNumSet = me.blockMaxZagSet[rowBlock]
                let ph         = &me.h[me.compAcTab[componentId]]

                var k = 1
                while k < 64
                {
                    s = me.huffDecode(ph)
                    var r = s >> 4
                    s &= 0x0F

                    if s
                    {
                        if r
                        {
                            if (k + r) > 63:
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

                        r = cast(s32) me.getBits2(s)
                        if s > 15:
                            throw InvalidFormatError{"decode error"}

                        s         = (r < ExtendTest[s] ? r + ExtendOff[s] : r)
                        p[Zag[k]] = cast(s16) (s * q[k])
                    }
                    else
                    {
                        if r == 15
                        {
                            if (k + 15) > 63:
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

                me.blockMaxZagSet[rowBlock] = k
                rowBlock += 1
            }

            me.restartsLeft -= 1
        }
    }

    func idct(data: ^s16, buf: ^u8)
    {
        var pdstPtr = buf
        var dataPtr = data

        for #reverse rowctr in 0 to 7
        {
            // Original code:
            // if (dataPtr[1] | dataPtr[2] | dataPtr[3] | dataPtr[4] | dataPtr[5] | dataPtr[6] | dataPtr[7]) == 0
            let dataPtr64 = cast(^u64) dataPtr
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
        var psrcPtr = me.blockSeg[0]
        var pdstPtr = me.pSampleBuf

        for me.maxBlocksPerRow
        {
            Memory.copy(me.tempBlock, psrcPtr, 64'u64 * #sizeof(s16))
            idct(me.tempBlock, pdstPtr)
            psrcPtr += 64
            pdstPtr += 64
        }
    }

    mtd findEoi() throw
    {
        if !me.progressiveFlag
        {
            me.bitsLeft = 16
            me.getBits1(16)
            me.getBits1(16)
            me.processMarkers()
        }

        me.totalBytesRead -= me.inBufLeft
    }

    mtd decode(scanLineOfs: **void, scanLineLen: *u32)->s32 throw
    {
        if me.totalLinesLeft == 0:
            return Done

        if me.mcuLinesLeft == 0
        {
            if me.progressiveFlag:
                me.loadNextRow()
            else:
                me.decodeNextRow()
            if me.totalLinesLeft <= me.maxMcuSizeY:
                me.findEoi()
            me.transformRow()
            me.mcuLinesLeft = me.maxMcuSizeY
        }

        switch me.scanType
        {
        case YH2V2:
            if (me.mcuLinesLeft & 1) == 0
            {
                me.convertH2V2()
                dref scanLineOfs = me.scanLine0.buffer
            }
            else:
                dref scanLineOfs = me.scanLine1.buffer

        case YH2V1:
            me.convertH2V1()
            dref scanLineOfs = me.scanLine0.buffer

        case YH1V2:
            if (me.mcuLinesLeft & 1) == 0
            {
                me.convertH1V2()
                dref scanLineOfs = me.scanLine0.buffer
            }
            else:
                dref scanLineOfs = me.scanLine1.buffer

        case YH1V1:
            me.convertH1V1()
            dref scanLineOfs = me.scanLine0.buffer

        case GrayScale:
            me.convertGrayscale()
            dref scanLineOfs = me.scanLine0.buffer
        }

        dref scanLineLen = me.realDestBytesPerScanLine
        me.mcuLinesLeft -= 1
        me.totalLinesLeft -= 1

        return Okay
    }

    mtd convertGrayscale()
    {
        let row = me.maxMcuSizeY - me.mcuLinesLeft
        var d   = me.scanLine0.buffer
        var s   = me.pSampleBuf + row * 8
        for #reverse i in 1 to me.maxMcusPerRow
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
        let row = me.maxMcuSizeY - me.mcuLinesLeft
        var d   = me.scanLine0.buffer
        var s   = me.pSampleBuf + row * 8
        for #reverse i in 1 to me.maxMcusPerRow
        {
            for j in 8
            {
                let y  = cast(s32) s[j]
                let cb = s[64 + j]
                let cr = s[128 + j]

                d[2] = clamp(y + #wrap me.crr[cr])
                d[1] = clamp(y + #wrap ((me.crg[cr] + me.cbg[cb]) >> 16))
                d[0] = clamp(y + #wrap me.cbb[cb])
                d += 3
            }

            s += 64 * 3
        }
    }

    mtd convertH1V2()
    {
        let row = me.maxMcuSizeY - me.mcuLinesLeft
        var d0  = me.scanLine0.buffer
        var d1  = me.scanLine1.buffer

        var y: ^u8 = undefined
        if row < 8:
            y = me.pSampleBuf + row * 8
        else:
            y = me.pSampleBuf + 64 * 1 + (row & 7) * 8

        var c = me.pSampleBuf + 64 * 2 + (row >> 1) * 8
        for #reverse i in 1 to me.maxMcusPerRow
        {
            for j in 8
            {
                let cb = c[0 + j]
                let cr = c[64 + j]
                let rc = me.crr[cr]
                let gc = ((me.crg[cr] + me.cbg[cb]) >> 16)
                let bc = me.cbb[cb]
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
        let row = me.maxMcuSizeY - me.mcuLinesLeft
        var d0  = me.scanLine0.buffer
        var y   = me.pSampleBuf + row * 8
        var c   = me.pSampleBuf + 2 * 64 + row * 8

        for #reverse i in 1 to me.maxMcusPerRow
        {
            for l in 2
            {
                for j in 4
                {
                    let cb = c[0]
                    let cr = c[64]
                    let rc = me.crr[cr]
                    let gc = (me.crg[cr] + me.cbg[cb]) >> 16
                    let bc = me.cbb[cb]
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
        let row = me.maxMcuSizeY - me.mcuLinesLeft
        var d0  = me.scanLine0.buffer
        var d1  = me.scanLine1.buffer

        var y: ^u8 = undefined
        if row < 8:
            y = me.pSampleBuf + row * 8
        else:
            y = me.pSampleBuf + 64 * 2 + (row & 7) * 8
        var c = me.pSampleBuf + 64 * 4 + (row >> 1) * 8

        for #reverse i in 1 to me.maxMcusPerRow
        {
            for l in 2
            {
                for var j = 0; j < 8; j += 2
                {
                    let cb = c[0]
                    let cr = c[64]
                    let rc = me.crr[cr]
                    let gc = (me.crg[cr] + me.cbg[cb]) >> 16
                    let bc = me.cbb[cb]
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
        if me.compsInFrame == 1
        {
            me.scanType        = GrayScale
            me.maxBlocksPerMcu = 1
            me.maxMcuSizeX     = 8
            me.maxMcuSizeY     = 8
        }
        elif me.compsInFrame == 3
        {
            if (me.compSampH[1] != 1) or (me.compSampV[1] != 1) or (me.compSampH[2] != 1) or (me.compSampV[2] != 1):
                throw InvalidFormatError{"invalid samp factors"}

            if (me.compSampH[0] == 1) and (me.compSampV[0] == 1)
            {
                me.scanType        = YH1V1
                me.maxBlocksPerMcu = 3
                me.maxMcuSizeX     = 8
                me.maxMcuSizeY     = 8
            }
            elif (me.compSampH[0] == 2) and (me.compSampV[0] == 1)
            {
                me.scanType        = YH2V1
                me.maxBlocksPerMcu = 4
                me.maxMcuSizeX     = 16
                me.maxMcuSizeY     = 8
            }
            elif (me.compSampH[0] == 1) and (me.compSampV[0] == 2)
            {
                me.scanType        = YH1V2
                me.maxBlocksPerMcu = 4
                me.maxMcuSizeX     = 8
                me.maxMcuSizeY     = 16
            }
            elif (me.compSampH[0] == 2) and (me.compSampV[0] == 2)
            {
                me.scanType        = YH2V2
                me.maxBlocksPerMcu = 6
                me.maxMcuSizeX     = 16
                me.maxMcuSizeY     = 16
            }
            else:
                throw InvalidFormatError{"invalid samp factors"}
        }
        else:
            throw UnsupportedFormatError{"unsupported color space"}

        me.maxMcusPerRow = (me.imageSizeX + (me.maxMcuSizeX - 1)) / me.maxMcuSizeX
        me.maxMcusPerCol = (me.imageSizeY + (me.maxMcuSizeY - 1)) / me.maxMcuSizeY

        if me.scanType == GrayScale:
            me.destBytesPerPixel = 1
        else:
            me.destBytesPerPixel = 4

        me.destBytesPerScanLine     = ((me.imageSizeX + 15) & 0xFFF0) * me.destBytesPerPixel
        me.realDestBytesPerScanLine = (me.imageSizeX * me.destBytesPerPixel)

        me.scanLine0.resize(cast(u64) me.destBytesPerScanLine + 8)
        Memory.clear(me.scanLine0.buffer, cast(u64) me.destBytesPerScanLine)

        me.scanLine1.resize(cast(u64) me.destBytesPerScanLine + 8)
        Memory.clear(me.scanLine1.buffer, cast(u64) me.destBytesPerScanLine)

        me.maxBlocksPerRow = me.maxMcusPerRow * me.maxBlocksPerMcu
        if me.maxBlocksPerRow > MaxBlocksPerRow:
            throw InvalidFormatError{"assertion error"}

        me.quantBuf.resize(cast(u64) me.maxBlocksPerRow * 64 * #sizeof(s16) + 8)
        for i in me.maxBlocksPerRow
        {
            me.blockSeg[i]       = cast(^s16) (me.quantBuf.buffer + i * 64 * #sizeof(s16))
            me.blockMaxZagSet[i] = 64
        }

        me.sampleBuf.resize(cast(u64) me.maxBlocksPerRow * 64 + 8)
        me.pSampleBuf = me.sampleBuf.buffer

        me.totalLinesLeft = me.imageSizeY
        me.mcuLinesLeft   = 0
        me.createLookUps()
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
            me.crr[i] = (fix[0] * k + (1 << (16 - 1))) >> 16
            me.cbb[i] = (fix[1] * k + (1 << (16 - 1))) >> 16
            me.crg[i] = (fix[2] * k)
            me.cbg[i] = (fix[3] * k + (1 << (16 - 1)))
        }
    }

    mtd initScan()->bool throw
    {
        let res = me.locateSosMarker()
        if !res:
            return false

        me.calcMcuBlockOrder()
        me.checkHuffTables()
        me.checkQuantTables()

        Memory.clear(me.lastDcVal, cast(u64) me.compsInFrame * #sizeof(u32))

        me.eobRun = 0
        if me.restartInterval
        {
            me.restartsLeft   = me.restartInterval
            me.nextRestartNum = 0
        }

        me.fixInBuffer()
        return true
    }

    mtd stuffChar(q: u8)
    {
        me.inBufOfs -= 1
        dref me.inBufOfs = q
        me.inBufLeft += 1
    }

    mtd fixInBuffer()
    {
        if me.bitsLeft == 16:
            me.stuffChar(cast(u8) ((me.bitBuf >> 16) & 0xFF))
        if me.bitsLeft >= 8:
            me.stuffChar(cast(u8) ((me.bitBuf >> 24) & 0xFF))

        me.stuffChar(cast(u8) (me.bitBuf & 0xFF))
        me.stuffChar(cast(u8) ((me.bitBuf >> 8) & 0xFF))
        me.bitsLeft = 16

        me.getBits2(16)
        me.getBits2(16)
    }

    mtd checkQuantTables() throw
    {
        for i in me.compsInScan
        {
            if me.quant[me.compQuant[me.compList[i]]].count == 0:
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
            for var i = 1; i <= me.huffNum[idx].buffer[l]; i += 1
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
            let i = me.huffVal[idx].buffer[p]
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
                    if (cde & 0x8000) == 0:
                        currentEntry -= 1

                    if hs.tree[-currentEntry - 1] == 0
                    {
                        hs.tree[-currentEntry - 1] = nextFreeEntry
                        currentEntry               = nextFreeEntry
                        nextFreeEntry -= 2
                    }
                    else:
                        currentEntry = hs.tree[-currentEntry - 1]

                    cde <<= 1
                }

                if (cde & 0x8000) == 0:
                    currentEntry -= 1
                hs.tree[-currentEntry - 1] = i
            }

            p += 1
        }
    }

    mtd checkHuffTables() throw
    {
        for i in me.compsInScan
        {
            if (me.spectralStart == 0) and (me.huffNum[me.compDcTab[me.compList[i]]].count == 0):
                throw InvalidFormatError{"undefined huff table"}

            if (me.spectralEnd > 0) and (me.huffNum[me.compAcTab[me.compList[i]]].count == 0):
                throw InvalidFormatError{"undefined huff table"}
        }

        for i in MaxHuffTables
        {
            if me.huffNum[i].count
            {
                me.makeHuffTable(i, &me.h[i])
            }
        }

        for i in me.blocksPerMcu
        {
            me.dcHuffSeg[i] = &me.h[me.compDcTab[me.mcuOrg[i]]]
            me.acHuffSeg[i] = &me.h[me.compAcTab[me.mcuOrg[i]]]
            me.component[i] = cast(*u32) &me.lastDcVal[me.mcuOrg[i]]
        }
    }

    mtd calcMcuBlockOrder()
    {
        var maxSampH = 1
        var maxSampV = 1

        for cid in me.compsInFrame
        {
            maxSampH = Math.max(maxSampH, me.compSampH[cid])
            maxSampV = Math.max(maxSampV, me.compSampV[cid])
        }

        for cid in me.compsInFrame
        {
            me.compBlocksH[cid] = ((((me.imageSizeX * me.compSampH[cid]) + (maxSampH - 1)) / maxSampH) + 7) / 8
            me.compBlocksV[cid] = ((((me.imageSizeY * me.compSampV[cid]) + (maxSampV - 1)) / maxSampV) + 7) / 8
        }

        if me.compsInScan == 1
        {
            me.mcusPerRow = me.compBlocksH[me.compList[0]]
            me.mcusPerCol = me.compBlocksV[me.compList[0]]
        }
        else
        {
            me.mcusPerRow = (((me.imageSizeX + 7) / 8) + (maxSampH - 1)) / maxSampH
            me.mcusPerCol = (((me.imageSizeY + 7) / 8) + (maxSampV - 1)) / maxSampV
        }

        if me.compsInScan == 1
        {
            me.mcuOrg[0]    = me.compList[0]
            me.blocksPerMcu = 1
        }
        else
        {
            me.blocksPerMcu = 0
            for cNum in me.compsInScan
            {
                let cid        = me.compList[cNum]
                var num_blocks = me.compSampH[cid] * me.compSampV[cid]
                while num_blocks
                {
                    me.mcuOrg[me.blocksPerMcu] = cid
                    me.blocksPerMcu += 1
                    num_blocks -= 1
                }
            }
        }
    }

    func coeffBufOpen(blkNumX, blkNumY, blkLenX, blkLenY: s32)->CoeffBuf
    {
        var result: retval
        using result
        blockNumX = blkNumX
        blockNumY = blkNumY
        blockLenX = blkLenX
        blockLenY = blkLenY
        blockSize = (blkLenX * blkLenY) * #sizeof(s16)
        pdata.resize(cast(u64) blockSize * blkNumX * blkNumY)
        return result
    }

    mtd decodeBlockDcFirst(cmpId: u32, blkX, blkY: s32) throw
    {
        var p = coeffBufGetp(me.dcCoeffs[cmpId], blkX, blkY)
        var s = me.huffDecode(&me.h[me.compDcTab[cmpId]])
        if s != 0
        {
            let r = me.getBits2(s)
            s = (r < ExtendTest[s] ? r + ExtendOff[s] : cast(s32) r)
        }

        s += me.lastDcVal[cmpId]
        me.lastDcVal[cmpId] = s
        p[0]                = cast(s16) (s << me.successiveLow)
    }

    mtd decodeBlockDcRefine(cmpId: u32, blkX, blkY: s32) throw
    {
        if me.getBits2(1)
        {
            var p = coeffBufGetp(me.dcCoeffs[cmpId], blkX, blkY)
            p[0] |= (1's16 << me.successiveLow)
        }
    }

    mtd decodeBlockAcFirst(cmpId: u32, blkX, blkY: s32) throw
    {
        if me.eobRun
        {
            me.eobRun -= 1
            return
        }

        var p = coeffBufGetp(me.acCoeffs[cmpId], blkX, blkY)

        for var k = me.spectralStart; k <= me.spectralEnd; k += 1
        {
            var s = me.huffDecode(&me.h[me.compAcTab[cmpId]])
            var r = s >> 4
            s &= 0x0F

            if s
            {
                k += r
                if k > 63:
                    throw InvalidFormatError{"decode error"}
                r = me.getBits2(s)
                if s > 15:
                    throw InvalidFormatError{"decode error"}

                s         = (r < ExtendTest[s] ? r + ExtendOff[s] : r)
                p[Zag[k]] = cast(s16) (s << me.successiveLow)
            }
            else
            {
                if r == 15
                {
                    k += 15
                    if k > 63:
                        throw InvalidFormatError{"decode error"}
                }
                else
                {
                    me.eobRun = 1 << r
                    if r:
                        me.eobRun += me.getBits2(r)
                    me.eobRun -= 1
                    break
                }
            }
        }
    }

    mtd decodeBlockAcRefine(cmpId: u32, blkX, blkY: s32) throw
    {
        let p1 = 1 << me.successiveLow
        let m1 = -1 << me.successiveLow
        var p  = coeffBufGetp(me.acCoeffs[cmpId], blkX, blkY)
        var k  = me.spectralStart
        if me.eobRun == 0
        {
            if me.spectralEnd >= 64:
                throw InvalidFormatError{"decode error"}

            while k <= me.spectralEnd
            {
                var s = me.huffDecode(&me.h[me.compAcTab[cmpId]])
                var r = s >> 4
                s &= 15

                if s
                {
                    if s != 1:
                        throw InvalidFormatError{"decode error"}
                    if me.getBits2(1):
                        s = p1
                    else:
                        s = m1
                }
                else
                {
                    if r != 15
                    {
                        me.eobRun = 1 << r
                        if r:
                            me.eobRun += me.getBits2(r)
                        break
                    }
                }

                for
                {
                    var thisCoef = p + Zag[k]
                    if dref thisCoef != 0
                    {
                        if me.getBits2(1)
                        {
                            if (dref thisCoef & cast(s16) p1) == 0
                            {
                                if dref thisCoef >= 0:
                                    dref thisCoef += cast(s16) p1
                                else:
                                    dref thisCoef += cast(s16) m1
                            }
                        }
                    }
                    else
                    {
                        r -= 1
                        if r < 0:
                            break
                    }

                    k += 1
                    if k > me.spectralEnd:
                        break
                }

                if s and (k < 64)
                {
                    p[Zag[k]] = cast(s16) s
                }

                k += 1
            }
        }

        if me.eobRun > 0
        {
            if me.spectralEnd >= 64:
                throw InvalidFormatError{"decode error"}
            while k <= me.spectralEnd
            {
                var thisCoef = p + Zag[k]
                if dref thisCoef != 0
                {
                    if me.getBits2(1)
                    {
                        if (dref thisCoef & cast(s16) p1) == 0
                        {
                            if dref thisCoef >= 0:
                                dref thisCoef += cast(s16) p1
                            else:
                                dref thisCoef += cast(s16) m1
                        }
                    }
                }

                k += 1
            }

            me.eobRun -= 1
        }
    }

    mtd decodeScan(fn: func(*Decoder, u32, s32, s32) throw) throw
    {
        var blockMcutX: [MaxComponents] s32
        var blockMcutY: [MaxComponents] s32

        for me.mcusPerCol
        {
            Memory.clear(blockMcutX, #sizeof(blockMcutX))
            for me.mcusPerRow
            {
                var blockMcuOfsX = 0
                var blockMcuOfsY = 0

                if me.restartInterval and (me.restartsLeft == 0):
                    me.restartProcess()

                for mcuBlock in me.blocksPerMcu
                {
                    let componentId = me.mcuOrg[mcuBlock]
                    fn(me, componentId, blockMcutX[componentId] + blockMcuOfsX, blockMcutY[componentId] + blockMcuOfsY)

                    if me.compsInScan == 1:
                        blockMcutX[componentId] += 1
                    else
                    {
                        blockMcuOfsX += 1
                        if blockMcuOfsX == me.compSampH[componentId]
                        {
                            blockMcuOfsX = 0
                            blockMcuOfsY += 1
                            if blockMcuOfsY == me.compSampV[componentId]
                            {
                                blockMcuOfsY = 0
                                blockMcutX[componentId] += me.compSampH[componentId]
                            }
                        }
                    }
                }

                me.restartsLeft -= 1
            }

            if me.compsInScan == 1:
                blockMcutY[me.compList[0]] += 1
            else
            {
                for n in me.compsInScan
                {
                    let componentId = me.compList[n]
                    blockMcutY[componentId] += me.compSampV[componentId]
                }
            }
        }
    }

    mtd initProgressive() throw
    {
        if me.compsInFrame == 4:
            throw UnsupportedFormatError{"unsupported color space"}

        for i in me.compsInFrame
        {
            me.dcCoeffs[i] = coeffBufOpen(me.maxMcusPerRow * me.compSampH[i], me.maxMcusPerCol * me.compSampV[i], 1, 1)
            me.acCoeffs[i] = coeffBufOpen(me.maxMcusPerRow * me.compSampH[i], me.maxMcusPerCol * me.compSampV[i], 8, 8)
        }

        for
        {
            let res = me.initScan()
            if !res:
                break

            let dcOnlyScan     = (me.spectralStart == 0)
            let refinementScan = (me.successiveHigh != 0)

            if (me.spectralStart > me.spectralEnd) or (me.spectralEnd > 63):
                throw InvalidFormatError{"bad sos spectral"}

            if dcOnlyScan
            {
                if me.spectralEnd:
                    throw InvalidFormatError{"bad sos spectral"}
            }
            elif me.compsInScan != 1
            {
                throw InvalidFormatError{"bad sos spectral"}
            }

            if refinementScan and (me.successiveLow != me.successiveHigh - 1):
                throw InvalidFormatError{"bad sos successive"}

            var decodeBlockFunc: func(*Decoder, u32, s32, s32) throw
            if dcOnlyScan
            {
                if refinementScan:
                    decodeBlockFunc = &decodeBlockDcRefine
                else:
                    decodeBlockFunc = &decodeBlockDcFirst
            }
            else
            {
                if refinementScan:
                    decodeBlockFunc = &decodeBlockAcRefine
                else:
                    decodeBlockFunc = &decodeBlockAcFirst
            }

            me.decodeScan(decodeBlockFunc)

            me.bitsLeft = 16

            me.getBits1(16)
            me.getBits1(16)
        }

        me.compsInScan = me.compsInFrame
        for i in me.compsInFrame:
            me.compList[i] = i
        me.calcMcuBlockOrder()
    }

    mtd initSequential() throw
    {
        let res = me.initScan()
        if !res:
            throw InvalidFormatError{"unexpected marker"}
    }

    #[Discardable]
    mtd getBits1(numBits: s32)->u32
    {
        let i = me.bitBuf >> cast(u32) (16 - numBits) & ((1'u32 << cast(u32) numBits) - 1)
        me.bitsLeft -= numBits
        if me.bitsLeft <= 0
        {
            me.bitBuf = rol(me.bitBuf, cast(u32) (numBits + me.bitsLeft))
            let c1 = me.getChar()
            let c2 = me.getChar()

            me.bitBuf = (me.bitBuf & 0xFFFF) | (cast(u32) c1 << 24) | (cast(u32) c2 << 16)
            me.bitBuf = rol(me.bitBuf, cast(u32) -me.bitsLeft)
            me.bitsLeft += 16
        }
        else:
            me.bitBuf = rol(me.bitBuf, cast(u32) numBits)

        return i
    }

    #[Discardable]
    mtd getBits2(numBits: s32)->u32
    {
        let i = (me.bitBuf >> cast(u32) (16 - numBits)) & ((1'u32 << cast(u32) numBits) - 1)
        me.bitsLeft -= numBits
        if me.bitsLeft <= 0
        {
            me.bitBuf = rol(me.bitBuf, cast(u32) (numBits + me.bitsLeft))
            let c1 = me.getOctet()
            let c2 = me.getOctet()

            me.bitBuf = (me.bitBuf & 0xFFFF) | (cast(u32) c1 << 24) | (cast(u32) c2 << 16)
            me.bitBuf = rol(me.bitBuf, cast(u32) -me.bitsLeft)
            me.bitsLeft += 16
        }
        else:
            me.bitBuf = rol(me.bitBuf, cast(u32) numBits)

        return i
    }

    mtd getOctet()->u8
    {
        var paddingFlag: bool
        var c            = me.getChar(&paddingFlag)
        if c == 0xFF
        {
            if paddingFlag:
                return 0xFF

            c = me.getChar(&paddingFlag)
            if paddingFlag
            {
                me.stuffChar(0xFF)
                return 0xFF
            }

            if c == 0x00:
                return 0xFF

            me.stuffChar(cast(u8) c)
            me.stuffChar(0xFF)
            return 0xFF
        }

        return cast(u8) c
    }

    #[Swag.Overload]
    mtd getChar(paddingFlag: *bool)->u32
    {
        if !me.inBufLeft
        {
            me.prepareInBuffer()
            if !me.inBufLeft
            {
                dref paddingFlag = true
                let t = me.temFlag
                me.temFlag ^= 1
                if t:
                    return 0xD9
                return 0xFF
            }
        }

        dref paddingFlag = false
        let c = dref me.inBufOfs
        me.inBufOfs += 1
        me.inBufLeft -= 1
        return c
    }

    #[Swag.Overload]
    mtd getChar()->u32
    {
        if !me.inBufLeft
        {
            me.prepareInBuffer()
            if !me.inBufLeft
            {
                let t = me.temFlag
                me.temFlag ^= 1
                if t:
                    return 0xD9
                return 0xFF
            }
        }

        let c = me.inBufOfs[0]
        me.inBufOfs += 1
        me.inBufLeft -= 1
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
                c = me.getBits1(8)
            }

            while c == 0xFF
            {
                c = me.getBits1(8)
            }
        }

        return cast(Marker) c
    }

    #[Discardable]
    mtd processMarkers()->Marker throw
    {
        for
        {
            let c = me.nextMarker()
            switch c
            {
            case M_SOF0, M_SOF1, M_SOF2, M_SOF3, M_SOF5, M_SOF6, M_SOF7, M_SOF9, M_SOF10, M_SOF11, M_SOF13, M_SOF14, M_SOF15, M_SOI, M_EOI, M_SOS:
                return c
            case M_JPG, M_RST0, M_RST1, M_RST2, M_RST3, M_RST4, M_RST5, M_RST6, M_RST7, M_TEM:
                throw InvalidFormatError{"unexpected marker"}
            case M_DAC:
                throw UnsupportedFormatError{"no arithmetic support"}
            case M_DHT:
                me.readDhtMarker()
            case M_DQT:
                me.readDqtMarker()
            case M_DRI:
                me.readDriMarker()
            default:
                if cast(u32) c & cast(u32) M_APP0 == cast(u32) M_APP0:
                    me.readAppMarker(c)
                else:
                    me.readVariableMarker(c)
            }
        }

        return Marker.M_ERROR
    }

    mtd readVariableMarker(_marker: Marker) throw
    {
        var left = me.getBits1(16)
        if left < 2:
            throw InvalidFormatError{"bad variable marker"}
        left -= 2

        @assert(!me.acceptMetaDatas)
        while left
        {
            me.getBits1(8)
            left -= 1
        }
    }

    mtd readDhtMarker() throw
    {
        var huffNumt: [17] u8
        var huffValt: [256] u8

        var left = me.getBits1(16)
        if left < 2:
            throw InvalidFormatError{"bad dht marker"}
        left -= 2

        while left
        {
            var index = me.getBits1(8)
            var count = 0
            huffNumt[0] = 0
            for i in 1 to 16
            {
                huffNumt[i] = cast(u8) me.getBits1(8)
                count += huffNumt[i]
            }

            if count > 255:
                throw InvalidFormatError{"bad dht counts"}
            for i in count
            {
                huffValt[i] = cast(u8) me.getBits1(8)
            }

            let i = 1 + 16 + count
            if left < i:
                throw InvalidFormatError{"bad dht marker"}

            left -= i
            if (index & 0x10) > 0x10:
                throw InvalidFormatError{"bad dht index"}

            index = (index & 0x0F) + ((index & 0x10) >> 4) * (MaxHuffTables >> 1)
            if index >= MaxHuffTables:
                throw InvalidFormatError{"bad dht index"}

            me.huffNum[index].resize(17)
            me.huffVal[index].resize(256)
            Memory.copy(me.huffNum[index].buffer, huffNumt, 17)
            Memory.copy(me.huffVal[index].buffer, huffValt, 256)
        }
    }

    mtd readDqtMarker() throw
    {
        var left = me.getBits1(16)
        if left < 2:
            throw InvalidFormatError{"bad dqt marker"}
        left -= 2

        while left
        {
            var n    = me.getBits1(8)
            let prec = n >> 4
            n &= 0x0F
            if n >= MaxQuantTables:
                throw InvalidFormatError{"bad dqt table"}

            me.quant[n].resize(64)
            for i in 64
            {
                var temp = me.getBits1(8)
                if prec:
                    temp = (temp << 8) + me.getBits1(8)
                me.quant[n].buffer[i] = cast(s16) temp
            }

            var i = 64 + 1
            if prec:
                i += 64
            if left < i:
                throw InvalidFormatError{"bad dqt length"}
            left -= i
        }
    }

    mtd readDriMarker() throw
    {
        if me.getBits1(16) != 4:
            throw InvalidFormatError{"bad dri length"}
        me.restartInterval = me.getBits1(16)
    }

    mtd readSofMarker() throw
    {
        let left = me.getBits1(16)
        if me.getBits1(8) != 8:
            throw InvalidFormatError{"bad precision"}

        me.imageSizeY = me.getBits1(16)
        me.imageSizeX = me.getBits1(16)

        if (me.imageSizeY < 1) or (me.imageSizeY > MaxHeight):
            throw InvalidFormatError{"bad height"}
        if (me.imageSizeX < 1) or (me.imageSizeX > MaxWidth):
            throw InvalidFormatError{"bad width"}

        me.compsInFrame = me.getBits1(8)
        if me.compsInFrame > MaxComponents:
            throw InvalidFormatError{"too many components"}
        if left != (me.compsInFrame * 3 + 8):
            throw InvalidFormatError{"bad sof length"}

        for i in me.compsInFrame
        {
            me.compIdent[i] = me.getBits1(8)
            me.compSampH[i] = me.getBits1(4)
            me.compSampV[i] = me.getBits1(4)
            me.compQuant[i] = me.getBits1(8)
        }
    }

    mtd readAppMarker(marker: Marker) throw
    {
        var left = me.getBits1(16)
        if left < 2:
            throw InvalidFormatError{"Bad app marker"}
        left -= 2

        if !me.decOptions.decodeMetaDatas
        {
            while left
            {
                me.getBits1(8)
                left -= 1
            }
        }
        else
        {
            var meta: ImageMetaData
            meta.sig = cast(u32) marker - cast(u32) Marker.M_APP0

            while left
            {
                let c = cast(u8) me.getBits1(8)
                left -= 1
                if c == 0:
                    break
                meta.tag.add(c)
            }

            while left
            {
                let c = cast(u8) me.getBits1(8)
                meta.value.add(c)
                left -= 1
            }

            me.metaDatas.add(meta)
        }
    }

    mtd readSosMarker() throw
    {
        var left = me.getBits1(16)
        let n    = me.getBits1(8)
        me.compsInScan = n
        left -= 3

        if (left != (n * 2 + 3)) or (n < 1) or (n > MaxCompsInScan):
            throw InvalidFormatError{"bad sos length"}

        for i in n
        {
            let cc = me.getBits1(8)
            let c  = me.getBits1(8)
            left -= 2

            var ci = 0
            while ci < me.compsInFrame
            {
                if cc == me.compIdent[ci]:
                    break
                ci += 1
            }

            if ci >= me.compsInFrame:
                throw InvalidFormatError{"bad sos comp id"}

            me.compList[i]   = ci
            me.compDcTab[ci] = (c >> 4) & 15
            me.compAcTab[ci] = (c & 15) + (MaxHuffTables >> 1)
        }

        me.spectralStart  = me.getBits1(8)
        me.spectralEnd    = me.getBits1(8)
        me.successiveHigh = me.getBits1(4)
        me.successiveLow  = me.getBits1(4)

        if !me.progressiveFlag
        {
            me.spectralStart = 0
            me.spectralEnd   = 63
        }

        left -= 3
        while left
        {
            me.getBits1(8)
            left -= 1
        }
    }

    mtd locateSosMarker()->bool throw
    {
        let c = me.processMarkers()
        if c == .M_EOI:
            return false
        elif c != .M_SOS:
            throw InvalidFormatError{"unexpected marker"}
        me.readSosMarker()
        return true
    }

    mtd locateSoiMarker() throw
    {
        var lastRune = me.getBits1(8)
        var thisChar = me.getBits1(8)
        if (lastRune == 0xFF) and (thisChar == Marker.M_SOI):
            return
        var bytesLeft = 512
        for
        {
            bytesLeft -= 1
            if bytesLeft == 0:
                throw InvalidFormatError{"not a jpeg"}
            lastRune = thisChar
            thisChar = me.getBits1(8)
            if (lastRune == 0xFF) and (thisChar == Marker.M_SOI):
                break
        }

        thisChar = (me.bitBuf >> 8) & 0xFF
        if thisChar != 0xFF:
            throw InvalidFormatError{"not a jpeg"}
    }

    mtd locateSofMarker() throw
    {
        me.locateSoiMarker()
        let c = me.processMarkers()
        switch c
        {
        case M_SOF2:
            me.progressiveFlag = true
            try me.readSofMarker()
        case M_SOF0, M_SOF1:
            me.readSofMarker()
        case M_SOF9:
            throw UnsupportedFormatError{"no arithmetic support"}
        default:
            throw UnsupportedFormatError{"unsupported marker"}
        }
    }

    func memset16(p: ^void, c: u16, n: u32)
    {
        var ps = cast(^u16) p
        for n
        {
            dref ps = c
            ps += 1
        }
    }

    mtd prepareInBuffer()
    {
        me.inBufLeft = 0
        me.inBufOfs  = me.inBuf
        if me.eofReached:
            return

        for
        {
            var size = InBufSize - me.inBufLeft
            if size + me.streamSeek > me.streamLen
            {
                size          = me.streamLen - me.streamSeek
                me.eofReached = 1
            }

            let bytesRead = size
            Memory.copy(&me.inBuf[0] + me.inBufLeft, me.stream + me.streamSeek, cast(u64) size)
            me.streamSeek += size
            me.inBufLeft += bytesRead
            if me.inBufLeft >= InBufSize or me.eofReached:
                break
        }

        me.totalBytesRead += me.inBufLeft
        memset16(me.inBufOfs + me.inBufLeft, 0xD9FF, 64)
    }
}

public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        var ext = Path.getExtensionLowerCase(fileName)
        return ext == ".jpg" or ext == ".jpeg"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        var result: retval
        me.decOptions = options

        me.init(buffer)
        me.start()

        // Creates image
        result.init(me.imageSizeX, me.imageSizeY, PixelFormat.BGR8)
        if !options.decodePixels:
            return result
        result.allocPixels()

        me.doit(result.pixels.buffer)
        result.metaDatas = #move me.metaDatas

        return result
    }
}
