#global namespace jpg
using swag, core

const InBufSize         = 4096
const MaxHeight         = 8192
const MaxWidth          = 8192
const MaxComponents     = 4
const MaxQuantTables    = 4
const MaxHuffTables     = 8
const MaxBocksPerMcu    = 10
const MaxBlocksPerRow   = 6144
const MaxCompsInScan    = 4
const GrayScale         = 0
const YH1V1             = 1
const YH2V1             = 2
const YH1V2             = 3
const YH2V2             = 4
const Failed            = -1
const Done              = 1
const Okay              = 0
const ConstBits         = 13'u32
const Pass1Bits         = 2'u32
const Fix_0_298631336   = 2446
const Fix_0_390180644   = 3196
const Fix_0_541196100   = 4433
const Fix_0_765366865   = 6270
const Fix_0_899976223   = 7373
const Fix_1_175875602   = 9633
const Fix_1_501321110   = 12299
const Fix_1_847759065   = 15137
const Fix_1_961570560   = 16069
const Fix_2_053119869   = 16819
const Fix_2_562915447   = 20995
const Fix_3_072711026   = 25172

struct HuffTables
{
    lookUp:    [256] s32
    codeSize:  [256] u8
    tree:      [512] s32
}

struct CoeffBuf
{
    pdata:     Array'u8
    blockNumX: s32
    blockNumY: s32
    blockLenX: s32
    blockLenY: s32
    blockSize: s32
}

#[opaque]
public struct Decoder
{
    stream:             const *u8
    inBufOfs:           *u8
    pSampleBuf:         *u8

    imageSizeX:         s32
    imageSizeY:         s32
    streamLen:          s32
    streamSeek:         s32
    bitBuf:             u32
    bitsLeft:           s32
    inBufLeft:          s32
    temFlag:            s32
    eofReached:         s32
    totalBytesRead:     s32
    compsInFrame:       s32
    restartInterval:    s32
    scanType:           s32
    maxBlocksPerMcu:    s32
    maxMcuSizeX:        s32
    maxMcuSizeY:        s32
    blocksPerMcu:       s32
    maxMcusPerRow:      s32
    maxMcusPerCol:      s32
    maxBlocksPerRow:    s32
    totalLinesLeft:     s32
    mcuLinesLeft:       s32
    spectralStart:      s32
    spectralEnd:        s32
    successiveLow:      s32
    successiveHigh:     s32
    mcusPerRow:         s32
    mcusPerCol:         s32
    eobRun:             s32
    restartsLeft:       s32
    nextRestartNum:     s32
    compsInScan:        s32

    destBytesPerPixel:        s32
    realDestBytesPerScanLine: s32
    destBytesPerScanLine:     s32

    acceptMetaDatas: bool
    progressiveFlag: bool

    inBuf:          [InBufSize + 128] u8
    compSampH:      [MaxComponents] s32
    compSampV:      [MaxComponents] s32
    compQuant:      [MaxComponents] s32
    compIdent:      [MaxComponents] s32
    blockMaxZagSet: [MaxBlocksPerRow] s32
    blockSeg:       [MaxBlocksPerRow] *s16
    compList:       [MaxCompsInScan] s32
    compDcTab:      [MaxComponents] s32
    compAcTab:      [MaxComponents] s32
    compBlocksH:    [MaxComponents] s32
    compBlocksV:    [MaxComponents] s32
    mcuOrg:         [MaxBocksPerMcu] s32
    h:              [MaxHuffTables] HuffTables
    dcHuffSeg:      [MaxBocksPerMcu] *HuffTables
    acHuffSeg:      [MaxBocksPerMcu] *HuffTables
    blockMcuY:      [MaxComponents] s32
    component:      [MaxBocksPerMcu] *u32
    lastDcVal:      [MaxComponents] u32
    dcCoeffs:       [MaxComponents] CoeffBuf
    acCoeffs:       [MaxComponents] CoeffBuf
    tempBlock:      [64] s16
    crr:            [256] s32
    cbb:            [256] s32
    crg:            [256] s32
    cbg:            [256] s32

    huffNum:        [MaxHuffTables] Array'u8
    huffVal:        [MaxHuffTables] Array'u8
    quant:          [MaxQuantTables] Array's16
    scanLine0:      Array'u8
    scanLine1:      Array'u8
    quantBuf:       Array'u8
    sampleBuf:      Array'u8
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
    M_APP1  = 0xE1
    M_APP15 = 0xEF
    M_JPG0  = 0xF0
    M_JPG13 = 0xFD
    M_COM   = 0xFE
    M_TEM   = 0x01
    M_ERROR = 0x100
}

const ExtendOff: [16] s32 = @[
    0,
    ((-1) << 1) + 1,
    ((-1) << 2) + 1,
    ((-1) << 3) + 1,
    ((-1) << 4) + 1,
    ((-1) << 5) + 1,
    ((-1) << 6) + 1,
    ((-1) << 7) + 1,
    ((-1) << 8) + 1,
    ((-1) << 9) + 1,
    ((-1) << 10) + 1,
    ((-1) << 11) + 1,
    ((-1) << 12) + 1,
    ((-1) << 13) + 1,
    ((-1) << 14) + 1,
    ((-1) << 15) + 1
]

const ExtendTest: [16] s32 = @[
    0, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020,
    0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800,
    0x1000, 0x2000, 0x4000
]

const Zag: [64] u32 = @[
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21,
    28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37,
    44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47,
    55, 62, 63,
]

#[inline, safety("of", false)]
{
    func descale(x: s32, n: u32)->s16 => cast(s16) ((x + (1's32 << (n - 1))) >> n)
    func rol(i, j: u32)->u32          => cast(u32) ((i << j) | (i >> (32 - j)))
    func clamp(i: u32)->u8            => cast(u8) ((i &0xFFFFFF00) ? (((~i) >> 31) & 0xFF) : i)
    func clampd(i: s32)->u8           => cast(u8) ((i &0xFF00) ? (((~i) >> 15) & 0xFF) : i)
}

impl Decoder
{
    func init(using self, buffer: const [..] u8) throw
    {
        @init(self)
        stream = @dataof(buffer)
        streamLen = cast(s32) @countof(buffer)

        inBufOfs = inBuf
        prepareInBuffer()
        bitsLeft = 16
        getBits1(16)
        getBits1(16)
        loop i: MaxBlocksPerRow blockMaxZagSet[i] = 64

        try locateSofMarker()
    }

    func start(using self) throw
    {
        try
        {
            initFrame()
            if progressiveFlag
                initProgressive()
            else
                initSequential()
        }
    }

    func doit(using self, buf: *u8) throw
    {
        var scanLineOfs: *void
        var scanLineLen: u32

        // Gray scale (1BPP)
        if compsInFrame == 1
        {
            pdest := buf
            pdest += (imageSizeY - 1) * imageSizeX * 3
            while true
            {
                if try decode(&scanLineOfs, &scanLineLen)
                    break

                pdest1 := pdest
                psrc := cast(*u8) scanLineOfs
                loop imageSizeX
                {
                    pdest1[0] = :psrc
                    pdest1[1] = :psrc
                    pdest1[2] = :psrc
                    pdest1 += 3
                    psrc += 1
                }

                pdest -= imageSizeX * 3
            }
        }

        // 3 BPP
        else
        {
            pdest := buf
            pdest += (imageSizeY - 1) * imageSizeX * 3
            while true
            {
                if try decode(&scanLineOfs, &scanLineLen)
                    break
                memory.copy(pdest, scanLineOfs, cast(uint) imageSizeX * 3)
                pdest -= imageSizeX * 3
            }
        }
    }

    func coeffBufGetp(cb: CoeffBuf, blkX, blkY: s32)->*s16
    {
        debug.assert(blkX < cb.blockNumX)
        debug.assert(blkY < cb.blockNumY)
        return cast(*s16) (cb.pdata.buffer + blkX * cb.blockSize + blkY * (cb.blockSize * cb.blockNumX))
    }

    func loadNextRow(using self) throw
    {
        var blockMcuX: [MaxComponents] s32

        rowBlock := 0
        loop mcusPerRow
        {
            blockMcuOfsX := 0
            blockMcuOfsY := 0
            loop mcuBlock: blocksPerMcu
            {
                componentId := mcuOrg[mcuBlock]

                p   := blockSeg[rowBlock]
                q   := quant[compQuant[componentId]]
                pAC := coeffBufGetp(acCoeffs[componentId], blockMcuX[componentId] + blockMcuOfsX, blockMcuY[componentId] + blockMcuOfsY)
                pDC := coeffBufGetp(dcCoeffs[componentId], blockMcuX[componentId] + blockMcuOfsX, blockMcuY[componentId] + blockMcuOfsY)

                p[0] = pDC[0]
                memory.copy(&p[1], &pAC[1], 63'uint * @sizeof(s16))

                i := 63
                while i > 0
                {
                    if p[Zag[i]]
                        break
                    i -= 1
                }

                while i >= 0
                {
                    if p[Zag[i]]
                        p[Zag[i]] *= q[i]
                    i -= 1
                }

                rowBlock += 1
                if compsInScan == 1
                    blockMcuX[componentId] += 1
                else
                {
                    blockMcuOfsX += 1
                    if blockMcuOfsX == compSampH[componentId]
                    {
                        blockMcuOfsX = 0
                        blockMcuOfsY += 1
                        if blockMcuOfsY == compSampV[componentId]
                        {
                            blockMcuOfsY = 0
                            blockMcuX[componentId] += compSampH[componentId]
                        }
                    }
                }
            }
        }

        if compsInScan == 1
            blockMcuY[compList[0]] += 1
        else
        {
            loop component_num: compsInScan
            {
                componentId := compList[component_num]
                blockMcuY[componentId] += compSampV[componentId]
            }
        }
    }

    func restartProcess(using self) throw
    {
        i := 1536
        while i > 0
        {
            if getChar() == 0xFF
                break
            i -= 1
        }

        if i == 0
            throw "bad restart marker"

        c := 0
        while i > 0
        {
            c = getChar()
            if c != 0xFF
                break
            i -= 1
        }

        if i == 0 throw "bad restart marker"
        if c != (nextRestartNum + cast(u32) Marker.M_RST0) throw "bad restart marker"

        memory.clear(&lastDcVal, cast(uint) compsInFrame * @sizeof(u32))

        eobRun         = 0
        restartsLeft   = restartInterval
        nextRestartNum = (nextRestartNum + 1) & 7
        bitsLeft       = 16

        getBits2(16)
        getBits2(16)
    }

    func huffDecode(using self, ph: *HuffTables)->s32
    {
        symbol := ph.lookUp[(bitBuf >>,safe 8) & 0xFF]
        if symbol < 0
        {
            getBits2(8)
            while true
            {
                symbol = ph.tree[~symbol + (1 - getBits2(1))]
                if symbol >= 0
                    break
            }
        }
        else
        {
            getBits2(cast(s32) ph.codeSize[symbol])
        }

        return symbol
    }

    func decodeNextRow(using self) throw
    {
        rowBlock := 0
        loop mcusPerRow
        {
            if restartInterval && (restartsLeft == 0)
                try restartProcess()

            loop mcuBlock: blocksPerMcu
            {
                componentId := mcuOrg[mcuBlock]
                p           := blockSeg[rowBlock]
                q           := quant[compQuant[componentId]].buffer

                s := huffDecode(&h[compDcTab[componentId]])
                if s != 0
                {
                    r := cast(s32) getBits2(s)
                    s = r < ExtendTest[s] ? r + ExtendOff[s] : r
                }

                s += bitcast(s32) lastDcVal[componentId]
                lastDcVal[componentId] = bitcast(u32) s

                p[0] = cast(s16) (s * q[0])
                prevNumSet := blockMaxZagSet[rowBlock]
                ph := &h[compAcTab[componentId]]

                k := 1
                while k < 64
                {
                    s = huffDecode(ph)
                    r := s >>,safe 4
                    s &= 0x0F

                    if s
                    {
                        if r
                        {
                            if (k + r) > 63 throw "decode error"
                            if k < prevNumSet
                            {
                                n := math.min(r, prevNumSet - k)
                                kt := k
                                while n
                                {
                                    p[Zag[kt]] = 0
                                    n -= 1
                                    kt += 1
                                }
                            }

                            k += r
                        }

                        r = cast(s32) getBits2(s)
                        if s > 15
                            throw "decode error"

                        s = (r < ExtendTest[s] ? r + ExtendOff[s] : r)
                        p[Zag[k]] = cast(s16) (s * q[k])
                    }
                    else
                    {
                        if r == 15
                        {
                            if (k + 15) > 63
                                throw "decode error"

                            if k < prevNumSet
                            {
                                n := math.min(16'u32, cast(u32) (prevNumSet - k))
                                kt := k
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
                    kt := k
                    while kt < prevNumSet
                    {
                        p[Zag[kt]] = 0
                        kt += 1
                    }
                }

                blockMaxZagSet[rowBlock] = k
                rowBlock += 1
            }

            restartsLeft -= 1
        }
    }

    func idct(using self, data: *s16, buf: *u8)
    {
        pdstPtr := buf
        dataPtr := data
        for rowctr := 8 - 1; rowctr >= 0; rowctr -= 1
        {
            if (dataPtr[1] | dataPtr[2] | dataPtr[3] | dataPtr[4] | dataPtr[5] | dataPtr[6] | dataPtr[7]) == 0
            {
                dcval := cast(s16) (dataPtr[0] << Pass1Bits)
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

            z2 := cast(s32) dataPtr[2]
            z3 := cast(s32) dataPtr[6]

            z1   := (z2 + z3) * Fix_0_541196100
            tmp2 := z1 + (z3 * -Fix_1_847759065)
            tmp3 := z1 + (z2 * Fix_0_765366865)

            tmp0 := (cast(s32) dataPtr[0] + cast(s32) dataPtr[4]) << ConstBits
            tmp1 := (cast(s32) dataPtr[0] - cast(s32) dataPtr[4]) << ConstBits

            tmp10 := tmp0 + tmp3
            tmp13 := tmp0 - tmp3
            tmp11 := tmp1 + tmp2
            tmp12 := tmp1 - tmp2

            tmp0 = cast(s32) dataPtr[7]
            tmp1 = cast(s32) dataPtr[5]
            tmp2 = cast(s32) dataPtr[3]
            tmp3 = cast(s32) dataPtr[1]

            z1 = tmp0 + tmp3
            z2 = tmp1 + tmp2
            z3 = tmp0 + tmp2
            z4 := tmp1 + tmp3
            z5 := (z3 + z4) * Fix_1_175875602

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
        for rowctr := 8 - 1; rowctr >= 0; rowctr -= 1
        {
            if (dataPtr[8 * 1] | dataPtr[8 * 2] | dataPtr[8 * 3] | dataPtr[8 * 4] | dataPtr[8 * 5] | dataPtr[8 * 6] | dataPtr[8 * 7]) == 0
            {
                dcval := descale(cast(s32) dataPtr[0], Pass1Bits + 3)
                dcval +=,safe 128
                dcval = math.clamp(dcval, 0's16, 255's16)

                pdstPtr[8 * 0] = cast(u8) dcval
                pdstPtr[8 * 1] = cast(u8) dcval
                pdstPtr[8 * 2] = cast(u8) dcval
                pdstPtr[8 * 3] = cast(u8) dcval
                pdstPtr[8 * 4] = cast(u8) dcval
                pdstPtr[8 * 5] = cast(u8) dcval
                pdstPtr[8 * 6] = cast(u8) dcval
                pdstPtr[8 * 7] = cast(u8) dcval

                dataPtr += 1
                pdstPtr += 1
                continue
            }

            z2 := cast(s32) dataPtr[8 * 2]
            z3 := cast(s32) dataPtr[8 * 6]

            z1   := (z2 + z3) * Fix_0_541196100
            tmp2 := z1 + (z3 * -Fix_1_847759065)
            tmp3 := z1 + (z2 * Fix_0_765366865)

            tmp0 := (cast(s32) dataPtr[8 * 0] + cast(s32) dataPtr[8 * 4]) << ConstBits
            tmp1 := (cast(s32) dataPtr[8 * 0] - cast(s32) dataPtr[8 * 4]) << ConstBits

            tmp10 := tmp0 + tmp3
            tmp13 := tmp0 - tmp3
            tmp11 := tmp1 + tmp2
            tmp12 := tmp1 - tmp2

            tmp0 = cast(s32) dataPtr[8 * 7]
            tmp1 = cast(s32) dataPtr[8 * 5]
            tmp2 = cast(s32) dataPtr[8 * 3]
            tmp3 = cast(s32) dataPtr[8 * 1]

            z1 = tmp0 + tmp3
            z2 = tmp1 + tmp2
            z3 = tmp0 + tmp2
            z4 := tmp1 + tmp3
            z5 := (z3 + z4) * Fix_1_175875602

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

            i := descale(tmp10 + tmp3, ConstBits + Pass1Bits + 3) + 128
            pdstPtr[8 * 0] = clampd(i)

            i = descale(tmp10 - tmp3, ConstBits + Pass1Bits + 3) + 128
            pdstPtr[8 * 7] = clampd(i)

            i = descale(tmp11 + tmp2, ConstBits + Pass1Bits + 3) + 128
            pdstPtr[8 * 1] = clampd(i)

            i = descale(tmp11 - tmp2, ConstBits + Pass1Bits + 3) + 128
            pdstPtr[8 * 6] = clampd(i)

            i = descale(tmp12 + tmp1, ConstBits + Pass1Bits + 3) + 128
            pdstPtr[8 * 2] = clampd(i)

            i = descale(tmp12 - tmp1, ConstBits + Pass1Bits + 3) + 128
            pdstPtr[8 * 5] = clampd(i)

            i = descale(tmp13 + tmp0, ConstBits + Pass1Bits + 3) + 128
            pdstPtr[8 * 3] = clampd(i)

            i = descale(tmp13 - tmp0, ConstBits + Pass1Bits + 3) + 128
            pdstPtr[8 * 4] = clampd(i)

            dataPtr, pdstPtr += 1
        }
    }

    func transformRow(using self)
    {
        psrcPtr := blockSeg[0]
        pdstPtr := pSampleBuf

        loop i: maxBlocksPerRow..1
        {
            memory.copy(tempBlock, psrcPtr, 64'uint * @sizeof(s16))
            idct(tempBlock, pdstPtr)
            psrcPtr += 64
            pdstPtr += 64
        }
    }

    func findEoi(using self) throw
    {
        if !progressiveFlag
        {
            bitsLeft = 16
            getBits1(16)
            getBits1(16)
            try processMarkers()
        }

        totalBytesRead -= inBufLeft
    }

    func decode(using self, scanLineOfs: **void, scanLineLen: *u32)->s32 throw
    {
        if totalLinesLeft == 0
            return Done

        if mcuLinesLeft == 0
        {
            if progressiveFlag
                try loadNextRow()
            else
                try decodeNextRow()
            if totalLinesLeft <= maxMcuSizeY
                try findEoi()
            transformRow()
            mcuLinesLeft = maxMcuSizeY
        }

        switch scanType
        {
        case YH2V2:
            if (mcuLinesLeft & 1) == 0
            {
                convertH2V2()
                :scanLineOfs = scanLine0.buffer
            }
            else
                :scanLineOfs = scanLine1.buffer

        case YH2V1:
            convertH2V1()
            :scanLineOfs = scanLine0.buffer

        case YH1V2:
            if (mcuLinesLeft & 1) == 0
            {
                convertH1V2()
                :scanLineOfs = scanLine0.buffer
            }
            else
                :scanLineOfs = scanLine1.buffer

        case YH1V1:
            convertH1V1()
            :scanLineOfs = scanLine0.buffer

        case GrayScale:
            convertGrayscale()
            :scanLineOfs = scanLine0.buffer
        }

        :scanLineLen = realDestBytesPerScanLine
        mcuLinesLeft -= 1
        totalLinesLeft -= 1

        return Okay
    }

    func convertGrayscale(using self)
    {
        row := maxMcuSizeY - mcuLinesLeft
        d := scanLine0.buffer
        s := pSampleBuf + row * 8
        loop i: maxMcusPerRow..1
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

    func convertH1V1(using self)
    {
        row := maxMcuSizeY - mcuLinesLeft
        d := scanLine0.buffer
        s := pSampleBuf + row * 8
        loop i: maxMcusPerRow..1
        {
            loop j: 8
            {
                y  := s[j]
                cb := s[64 + j]
                cr := s[128 + j]

                d[2] = clamp(y +,safe crr[cr])
                d[1] = clamp(y +,safe ((crg[cr] + cbg[cb]) >>,safe 16))
                d[0] = clamp(y +,safe cbb[cb])
                d += 3
            }

            s += 64 * 3
        }
    }

    func convertH1V2(using self)
    {
        row := maxMcuSizeY - mcuLinesLeft
        d0 := scanLine0.buffer
        d1 := scanLine1.buffer

        var y: *u8 = ?
        if row < 8
            y = pSampleBuf + row * 8
        else
            y = pSampleBuf + 64 * 1 + (row & 7) * 8

        c := pSampleBuf + 64 * 2 + (row >>,safe 1) * 8
        loop i: maxMcusPerRow..1
        {
            loop j: 8
            {
                cb := c[0 + j]
                cr := c[64 + j]
                rc := crr[cr]
                gc := ((crg[cr] + cbg[cb]) >>,safe 16)
                bc := cbb[cb]
                yy := y[j]

                d0[2] = clamp(yy +,safe rc)
                d0[1] = clamp(yy +,safe gc)
                d0[0] = clamp(yy +,safe bc)

                yy    = y[8 + j]
                d1[2] = clamp(yy +,safe rc)
                d1[1] = clamp(yy +,safe gc)
                d1[0] = clamp(yy +,safe bc)

                d0, d1 += 3
            }

            y, c += 64 * 4
        }
    }

    func convertH2V1(using self)
    {
        row := maxMcuSizeY - mcuLinesLeft
        d0 := scanLine0.buffer
        y := pSampleBuf + row * 8
        c := pSampleBuf + 2 * 64 + row * 8

        loop i: maxMcusPerRow..1
        {
            loop l: 2
            {
                loop j: 4
                {
                    cb := c[0]
                    cr := c[64]
                    rc := crr[cr]
                    gc := (crg[cr] + cbg[cb]) >>,safe 16
                    bc := cbb[cb]
                    yy := y[j << 1]

                    d0[2] = clamp(yy +,safe rc)
                    d0[1] = clamp(yy +,safe gc)
                    d0[0] = clamp(yy +,safe bc)

                    yy    = y[(j << 1) + 1]
                    d0[5] = clamp(yy +,safe rc)
                    d0[4] = clamp(yy +,safe gc)
                    d0[3] = clamp(yy +,safe bc)

                    d0 += 6

                    c += 1
                }

                y += 64
            }

            y += 64 * 4 - 64 * 2
            c += 64 * 4 - 8
        }
    }

    func convertH2V2(using self)
    {
        row := maxMcuSizeY - mcuLinesLeft
        d0 := scanLine0.buffer
        d1 := scanLine1.buffer

        var y: *u8 = ?
        if row < 8
            y = pSampleBuf + row * 8
        else
            y = pSampleBuf + 64 * 2 + (row & 7) * 8
        c := pSampleBuf + 64 * 4 + (row >> 1) * 8

        loop i: maxMcusPerRow..1
        {
            loop l: 2
            {
                for j := 0; j < 8; j += 2
                {
                    cb := c[0]
                    cr := c[64]
                    rc := crr[cr]
                    gc := (crg[cr] + cbg[cb]) >>,safe 16
                    bc := cbb[cb]
                    yy := y[j]

                    d0[2] = clamp(yy +,safe rc)
                    d0[1] = clamp(yy +,safe gc)
                    d0[0] = clamp(yy +,safe bc)

                    yy    = y[j + 1]
                    d0[5] = clamp(yy +,safe rc)
                    d0[4] = clamp(yy +,safe gc)
                    d0[3] = clamp(yy +,safe bc)

                    yy    = y[j + 8]
                    d1[2] = clamp(yy +,safe rc)
                    d1[1] = clamp(yy +,safe gc)
                    d1[0] = clamp(yy +,safe bc)

                    yy    = y[j + 8 + 1]
                    d1[5] = clamp(yy +,safe rc)
                    d1[4] = clamp(yy +,safe gc)
                    d1[3] = clamp(yy +,safe bc)

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

    func initFrame(using self) throw
    {
        if compsInFrame == 1
        {
            scanType        = GrayScale
            maxBlocksPerMcu = 1
            maxMcuSizeX     = 8
            maxMcuSizeY     = 8
        }
        else if compsInFrame == 3
        {
            if (compSampH[1] != 1) || (compSampV[1] != 1) || (compSampH[2] != 1) || (compSampV[2] != 1)
                throw "unsupported samp factors"

            if (compSampH[0] == 1) && (compSampV[0] == 1)
            {
                scanType        = YH1V1
                maxBlocksPerMcu = 3
                maxMcuSizeX     = 8
                maxMcuSizeY     = 8
            }
            else if (compSampH[0] == 2) && (compSampV[0] == 1)
            {
                scanType        = YH2V1
                maxBlocksPerMcu = 4
                maxMcuSizeX     = 16
                maxMcuSizeY     = 8
            }
            else if (compSampH[0] == 1) && (compSampV[0] == 2)
            {
                scanType        = YH1V2
                maxBlocksPerMcu = 4
                maxMcuSizeX     = 8
                maxMcuSizeY     = 16
            }
            else if (compSampH[0] == 2) && (compSampV[0] == 2)
            {
                scanType        = YH2V2
                maxBlocksPerMcu = 6
                maxMcuSizeX     = 16
                maxMcuSizeY     = 16
            }
            else
                throw "unsupported samp factors"
        }
        else
            throw "unsupported color space"

        maxMcusPerRow = (imageSizeX + (maxMcuSizeX - 1)) / maxMcuSizeX
        maxMcusPerCol = (imageSizeY + (maxMcuSizeY - 1)) / maxMcuSizeY

        if scanType == GrayScale
            destBytesPerPixel = 1
        else
            destBytesPerPixel = 4

        destBytesPerScanLine     = ((imageSizeX + 15) & 0xFFF0) * destBytesPerPixel
        realDestBytesPerScanLine = (imageSizeX * destBytesPerPixel)

        scanLine0.resize(cast(uint) destBytesPerScanLine + 8)
        memory.clear(scanLine0.buffer, cast(uint) destBytesPerScanLine)

        scanLine1.resize(cast(uint) destBytesPerScanLine + 8)
        memory.clear(scanLine1.buffer, cast(uint) destBytesPerScanLine)

        maxBlocksPerRow = maxMcusPerRow * maxBlocksPerMcu
        if maxBlocksPerRow > MaxBlocksPerRow
            throw "assertion error"

        quantBuf.resize(cast(uint) maxBlocksPerRow * 64 * @sizeof(s16) + 8)
        loop i: maxBlocksPerRow
        {
            blockSeg[i] = cast(*s16) (quantBuf.buffer + i * 64 * @sizeof(s16))
            blockMaxZagSet[i] = 64
        }

        sampleBuf.resize(cast(uint) maxBlocksPerRow * 64 + 8)
        pSampleBuf = sampleBuf.buffer

        totalLinesLeft = imageSizeY
        mcuLinesLeft   = 0
        createLookUps()
    }

    func createLookUps(using self)
    {
        var fix: [4] s32 = ?
        fix[0] = cast(s32) ((1.40200 / 2) * (1 << 16) + 0.5)
        fix[1] = cast(s32) ((1.77200 / 2) * (1 << 16) + 0.5)
        fix[2] = cast(s32) -((0.71414 / 2) * (1 << 16) + 0.5)
        fix[3] = cast(s32) -((0.34414 / 2) * (1 << 16) + 0.5)

        loop i: 255
        {
            k := (i * 2) - 256
            crr[i] = (fix[0] * k + (1 << (16 - 1))) >>,safe 16
            cbb[i] = (fix[1] * k + (1 << (16 - 1))) >>,safe 16
            crg[i] = (fix[2] * k)
            cbg[i] = (fix[3] * k + (1 << (16 - 1)))
        }
    }

    func initScan(using self)->bool throw
    {
        res := try locateSosMarker()
        if !res return false

        calcMcuBlockOrder()
        try checkHuffTables()
        try checkQuantTables()

        memory.clear(lastDcVal, cast(uint) compsInFrame * @sizeof(u32))

        eobRun = 0
        if restartInterval
        {
            restartsLeft    = restartInterval
            nextRestartNum = 0
        }

        fixInBuffer()
        return true
    }

    func stuffChar(using self, q: u8)
    {
        inBufOfs -= 1
        :inBufOfs = q
        inBufLeft += 1
    }

    func fixInBuffer(using self)
    {
        if bitsLeft == 16
            stuffChar(cast(u8) ((bitBuf >>,safe 16) & 0xFF))
        if bitsLeft >= 8
            stuffChar(cast(u8) ((bitBuf >>,safe 24) & 0xFF))

        stuffChar(cast(u8) (bitBuf & 0xFF))
        stuffChar(cast(u8) ((bitBuf >>,safe 8) & 0xFF))
        bitsLeft = 16

        getBits2(16)
        getBits2(16)
    }

    func checkQuantTables(using self) throw
    {
        loop i: compsInScan
        {
            if quant[compQuant[compList[i]]].count == 0
                throw "undefined quant table"
        }
    }

    func makeHuffTable(using self, idx: s32, hs: *HuffTables)
    {
        var huffSize: [257] u8
        var huffCode: [257] u32

        p := 0
        loop l: 1..16
        {
            for i := 1; i <= huffNum[idx].buffer[l]; i += 1
            {
                huffSize[p] = cast(u8) l
                p += 1
            }
        }

        huffSize[p] = 0
        lastp := p
        cde := 0
        si := huffSize[0]
        p = 0

        while huffSize[p]
        {
            while huffSize[p] == si
            {
                huffCode[p] = cde
                p, cde += 1
            }

            cde <<=,safe 1
            si += 1
        }

        memory.clear(hs.lookUp, @sizeof(hs.lookUp))
        memory.clear(hs.tree, @sizeof(hs.tree))
        memory.clear(hs.codeSize, @sizeof(hs.codeSize))

        nextFreeEntry := -1
        p = 0
        while p < lastp
        {
            i := huffVal[idx].buffer[p]
            cde = huffCode[p]
            codeSize := huffSize[p]
            hs.codeSize[i] = cast(u8) codeSize
            if codeSize <= 8
            {
                cde <<= (8 - codeSize)
                for l := 1 << (8 - codeSize); l > 0; l -= 1
                {
                    hs.lookUp[cde] = i
                    cde += 1
                }
            }
            else
            {
                subtree := (cde >>,safe (codeSize - 8)) & 0xFF
                currentEntry := hs.lookUp[subtree]
                if currentEntry == 0
                {
                    hs.lookUp[subtree] = nextFreeEntry
                    currentEntry = nextFreeEntry
                    nextFreeEntry -= 2
                }

                cde <<= (16 - (codeSize - 8))
                for l := codeSize; l > 9; l -= 1
                {
                    if (cde & 0x8000) == 0
                        currentEntry -= 1

                    if hs.tree[-currentEntry - 1] == 0
                    {
                        hs.tree[-currentEntry - 1] = nextFreeEntry
                        currentEntry = nextFreeEntry
                        nextFreeEntry -= 2
                    }
                    else
                        currentEntry = hs.tree[-currentEntry - 1]

                    cde <<= 1
                }

                if (cde & 0x8000) == 0
                    currentEntry -= 1
                hs.tree[-currentEntry - 1] = i
            }

            p += 1
        }
    }

    func checkHuffTables(using self) throw
    {
        loop i: compsInScan
        {
            if (spectralStart == 0) && (huffNum[compDcTab[compList[i]]].count == 0)
                throw "undefined huff table"

            if (spectralEnd > 0) && (huffNum[compAcTab[compList[i]]].count == 0)
                throw "undefined huff table"
        }

        loop i: MaxHuffTables
        {
            if huffNum[i].count
            {
                makeHuffTable(i, &h[i])
            }
        }

        loop i: blocksPerMcu
        {
            dcHuffSeg[i] = &h[compDcTab[mcuOrg[i]]]
            acHuffSeg[i] = &h[compAcTab[mcuOrg[i]]]
            component[i] = &lastDcVal[mcuOrg[i]]
        }
    }

    func calcMcuBlockOrder(using self)
    {
        maxSampH := 0
        maxSampV := 0

        loop cid: compsInFrame
        {
            if compSampH[cid] > maxSampH
                maxSampH = compSampH[cid]
            if compSampV[cid] > maxSampV
                maxSampV = compSampV[cid]
        }

        loop cid: compsInFrame
        {
            compBlocksH[cid] = ((((imageSizeX * compSampH[cid]) + (maxSampH - 1)) / maxSampH) + 7) / 8
            compBlocksV[cid] = ((((imageSizeY * compSampV[cid]) + (maxSampV - 1)) / maxSampV) + 7) / 8
        }

        if compsInScan == 1
        {
            mcusPerRow = compBlocksH[compList[0]]
            mcusPerCol = compBlocksV[compList[0]]
        }
        else
        {
            mcusPerRow = (((imageSizeX + 7) / 8) + (maxSampH - 1)) / maxSampH
            mcusPerCol = (((imageSizeY + 7) / 8) + (maxSampV - 1)) / maxSampV
        }

        if compsInScan == 1
        {
            mcuOrg[0] = compList[0]
            blocksPerMcu = 1
        }
        else
        {
            blocksPerMcu = 0
            loop cNum: compsInScan
            {
                cid := compList[cNum]
                num_blocks := compSampH[cid] * compSampV[cid]
                while num_blocks
                {
                    mcuOrg[blocksPerMcu] = cid
                    blocksPerMcu += 1
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
        blockSize  = (blkLenX * blkLenY) * @sizeof(s16)
        pdata.resize(cast(uint) blockSize * blkNumX * blkNumY)
        return result
    }

    #[safety("of", false)]
    func decodeBlockDcFirst(using self, cmpId: s32, blkX, blkY: s32) throw
    {
        p := coeffBufGetp(dcCoeffs[cmpId], blkX, blkY)
        s := huffDecode(&h[compDcTab[cmpId]])
        if s != 0
        {
            r := getBits2(s)
            s = (r < ExtendTest[s] ? r + ExtendOff[s] : r)
        }

        s += lastDcVal[cmpId]
        lastDcVal[cmpId] = s
        p[0] = cast(s16) (s << successiveLow)
    }

    #[safety("of", false)]
    func decodeBlockDcRefine(using self, cmpId: s32, blkX, blkY: s32) throw
    {
        if getBits2(1)
        {
            p := coeffBufGetp(dcCoeffs[cmpId], blkX, blkY)
            p[0] |= (1 << successiveLow)
        }
    }

    #[safety("of", false)]
    func decodeBlockAcFirst(using self, cmpId: s32, blkX, blkY: s32) throw
    {
        if eobRun
        {
            eobRun -= 1
            return
        }

        p := coeffBufGetp(acCoeffs[cmpId], blkX, blkY)

        for k := spectralStart; k <= spectralEnd; k += 1
        {
            s := huffDecode(&h[compAcTab[cmpId]])
            r := s >> 4
            s &= 0x0F

            if s
            {
                k += r
                if k > 63 throw "decode error"
                r = getBits2(s)
                if s > 15 throw "decode error"

                s = (r < ExtendTest[s] ? r + ExtendOff[s] : r)
                p[Zag[k]] = cast(s16) (s << successiveLow)
            }
            else
            {
                if r == 15
                {
                    k += 15
                    if k > 63
                        throw "decode error"
                }
                else
                {
                    eobRun = 1 << r
                    if r
                        eobRun += getBits2(r)
                    eobRun -= 1
                    break
                }
            }
        }
    }

    #[safety("of", false)]
    func decodeBlockAcRefine(using self, cmpId: s32, blkX, blkY: s32) throw
    {
        p1 := 1 << successiveLow
        m1 := (-1) << successiveLow
        p := coeffBufGetp(acCoeffs[cmpId], blkX, blkY)
        k := spectralStart
        if eobRun == 0
        {
            if spectralEnd >= 64 throw "decode error"

            while k <= spectralEnd
            {
                s := huffDecode(&h[compAcTab[cmpId]])
                r := s >> 4
                s &= 15

                if s
                {
                    if s != 1 throw "decode error"
                    if getBits2(1)
                        s = p1
                    else
                        s = m1
                }
                else
                {
                    if r != 15
                    {
                        eobRun = 1 << r
                        if r
                            eobRun += getBits2(r)
                        break
                    }
                }

                while true
                {
                    thisCoef := p + Zag[k]
                    if :thisCoef != 0
                    {
                        if getBits2(1)
                        {
                            if (:thisCoef & cast(s16) p1) == 0
                            {
                                if :thisCoef >= 0
                                    :thisCoef += cast(s16) p1
                                else
                                    :thisCoef += cast(s16) m1
                            }
                        }
                    }
                    else
                    {
                        r -= 1
                        if r < 0
                            break
                    }

                    k += 1
                    if k > spectralEnd
                        break
                }

                if s && (k < 64)
                {
                    p[Zag[k]] = cast(s16) s
                }

                k += 1
            }
        }

        if eobRun > 0
        {
            if spectralEnd >= 64 throw "decode error"
            while k <= spectralEnd
            {
                thisCoef := p + Zag[k]
                if :thisCoef != 0
                {
                    if getBits2(1)
                    {
                        if (:thisCoef & cast(s16) p1) == 0
                        {
                            if :thisCoef >= 0
                                :thisCoef += cast(s16) p1
                            else
                                :thisCoef += cast(s16) m1
                        }
                    }
                }

                k += 1
            }

            eobRun -= 1
        }
    }

    func decodeScan(using self, fn: func(*Decoder, s32, s32, s32) throw) throw
    {
        var blockMcutX: [MaxComponents] s32
        var blockMcutY: [MaxComponents] s32

        loop mcusPerCol
        {
            memory.clear(blockMcutX, @sizeof(blockMcutX))
            loop mcusPerRow
            {
                blockMcuOfsX := 0
                blockMcuOfsY := 0

                if restartInterval && (restartsLeft == 0)
                    try restartProcess()

                loop mcuBlock: blocksPerMcu
                {
                    componentId := mcuOrg[mcuBlock]
                    try fn(self, componentId, blockMcutX[componentId] + blockMcuOfsX, blockMcutY[componentId] + blockMcuOfsY)

                    if compsInScan == 1
                        blockMcutX[componentId] += 1
                    else
                    {
                        blockMcuOfsX += 1
                        if blockMcuOfsX == compSampH[componentId]
                        {
                            blockMcuOfsX = 0
                            blockMcuOfsY += 1
                            if blockMcuOfsY == compSampV[componentId]
                            {
                                blockMcuOfsY = 0
                                blockMcutX[componentId] += compSampH[componentId]
                            }
                        }
                    }
                }

                restartsLeft -= 1
            }

            if compsInScan == 1
                blockMcutY[compList[0]] += 1
            else
            {
                loop component_num: compsInScan
                {
                    componentId := compList[component_num]
                    blockMcutY[componentId] += compSampV[componentId]
                }
            }
        }
    }

    func initProgressive(using self) throw
    {
        if compsInFrame == 4 throw "unsupported color space"

        loop i: compsInFrame
        {
            dcCoeffs[i] = coeffBufOpen(maxMcusPerRow * compSampH[i], maxMcusPerCol * compSampV[i], 1, 1)
            acCoeffs[i] = coeffBufOpen(maxMcusPerRow * compSampH[i], maxMcusPerCol * compSampV[i], 8, 8)
        }

        while true
        {
            res := try initScan()
            if !res break

            dcOnlyScan     := (spectralStart == 0)
            refinementScan := (successiveHigh != 0)

            if (spectralStart > spectralEnd) || (spectralEnd > 63)
                throw "bad sos spectral"

            if dcOnlyScan
            {
                if spectralEnd
                    throw "bad sos spectral"
            }
            else if compsInScan != 1
            {
                throw "bad sos spectral"
            }

            if refinementScan && (successiveLow != successiveHigh - 1)
                throw "bad sos successive"

            var decodeBlockFunc: func(*Decoder, s32, s32, s32) throw
            if dcOnlyScan
            {
                if refinementScan
                    decodeBlockFunc = &decodeBlockDcRefine
                else
                    decodeBlockFunc = &decodeBlockDcFirst
            }
            else
            {
                if refinementScan
                    decodeBlockFunc = &decodeBlockAcRefine
                else
                    decodeBlockFunc = &decodeBlockAcFirst
            }

            try decodeScan(decodeBlockFunc)

            bitsLeft = 16

            getBits1(16)
            getBits1(16)
        }

        compsInScan = compsInFrame
        loop i: compsInFrame
            compList[i] = i
        calcMcuBlockOrder()
    }

    func initSequential(using self) throw
    {
        res := try initScan()
        if !res throw "unexpected marker"
    }

    #[safety("of", false), discardable]
    func getBits1(using self, numBits: s32)->u32
    {
        i := bitBuf >> cast(u32) ((16 - numBits)) & ((1'u32 << cast(u32) numBits) - 1)
        bitsLeft -= numBits
        if bitsLeft <= 0
        {
            bitBuf = rol(bitBuf, cast(u32) (numBits + bitsLeft))
            c1 := getChar()
            c2 := getChar()

            bitBuf = (bitBuf & 0xFFFF) | (cast(u32) c1 << 24) | (cast(u32) c2 << 16)
            bitBuf = rol(bitBuf, cast(u32) -bitsLeft)
            bitsLeft += 16
        }
        else
            bitBuf = rol(bitBuf, cast(u32) numBits)

        return i
    }

    #[safety("of", false), discardable]
    func getBits2(using self, numBits: s32)->u32
    {
        i := (bitBuf >> cast(u32) (16 - numBits)) & ((1'u32 << cast(u32) numBits) - 1)
        bitsLeft -= numBits
        if bitsLeft <= 0
        {
            bitBuf = rol(bitBuf, cast(u32) (numBits + bitsLeft))
            c1 := getOctet()
            c2 := getOctet()

            bitBuf = (bitBuf & 0xFFFF) | ((cast(u32) c1) << 24) | ((cast(u32) c2) << 16)
            bitBuf = rol(bitBuf, cast(u32) -bitsLeft)
            bitsLeft += 16
        }
        else
            bitBuf = rol(bitBuf, cast(u32) numBits)

        return i
    }

    func getOctet(using self)->u8
    {
        var paddingFlag: bool
        c := getChar(&paddingFlag)
        if c == 0xFF
        {
            if paddingFlag
                return 0xFF

            c = getChar(&paddingFlag)
            if paddingFlag
            {
                stuffChar(0xFF)
                return 0xFF
            }

            if c == 0x00
                return 0xFF

            stuffChar(cast(u8) c)
            stuffChar(0xFF)
            return 0xFF
        }

        return cast(u8) c
    }

    func getChar(using self, paddingFlag: *bool)->u32
    {
        if !inBufLeft
        {
            prepareInBuffer()
            if !inBufLeft
            {
                :paddingFlag = true
                t := temFlag
                temFlag ^= 1
                if t return 0xD9
                return 0xFF
            }
        }

        :paddingFlag = false
        c := :inBufOfs
        inBufOfs += 1
        inBufLeft -= 1
        return c
    }

    func getChar(using self)->u32
    {
        if !inBufLeft
        {
            prepareInBuffer()
            if !inBufLeft
            {
                t := temFlag
                temFlag ^= 1
                if t return 0xD9
                return 0xFF
            }
        }

        c := inBufOfs[0]
        inBufOfs += 1
        inBufLeft -= 1
        return c
    }

    func nextMarker(using self)->Marker
    {
        var c: u32

        bytes := 0
        while !c
        {
            while c != 0xFF
            {
                bytes += 1
                c = getBits1(8)
            }

            while c == 0xFF
            {
                c = getBits1(8)
            }
        }

        return cast(Marker) c
    }

    #[discardable]
    func processMarkers(using self)->Marker throw
    {
        while true
        {
            c := nextMarker()
            switch c
            {
            case M_SOF0, M_SOF1, M_SOF2, M_SOF3, M_SOF5, M_SOF6,
                 M_SOF7, M_SOF9, M_SOF10, M_SOF11, M_SOF13, M_SOF14,
                 M_SOF15, M_SOI, M_EOI, M_SOS:
                return c
            case M_JPG, M_RST0, M_RST1, M_RST2, M_RST3, M_RST4,
                 M_RST5, M_RST6, M_RST7, M_TEM:
                throw "unexpected marker"
            case M_DAC:
                throw "no arithmetic support"
            case M_DHT:
                try readDhtMarker()
            case M_DQT:
                try readDqtMarker()
            case M_DRI:
                try readDriMarker()
            default:
                try readVariableMarker(c)
            }
        }

        return Marker.M_ERROR
    }

    func readVariableMarker(using self, marker: Marker) throw
    {
        left := getBits1(16)
        if left < 2 throw "bad variable marker"
        left -= 2

        @assert(!acceptMetaDatas)
        while left
        {
            getBits1(8)
            left -= 1
        }
    }

    func readDhtMarker(using self) throw
    {
        var huffNumt: [17] u8
        var huffValt: [256] u8

        left := getBits1(16)
        if left < 2 throw "bad dht marker"
        left -= 2

        while left
        {
            index := getBits1(8)
            count := 0
            huffNumt[0] = 0
            loop i: 1..16
            {
                huffNumt[i] = cast(u8) getBits1(8)
                count += huffNumt[i]
            }

            if count > 255 throw "bad dht counts"
            loop i: count
            {
                huffValt[i] = cast(u8) getBits1(8)
            }

            i := 1 + 16 + count
            if left < i throw "bad dht marker"

            left -= i
            if (index & 0x10) > 0x10 throw "bad dht index"

            index = (index & 0x0F) + ((index & 0x10) >> 4) * (MaxHuffTables >> 1)
            if index >= MaxHuffTables throw "bad dht index"

            huffNum[index].resize(17)
            huffVal[index].resize(256)
            memory.copy(huffNum[index].buffer, huffNumt, 17)
            memory.copy(huffVal[index].buffer, huffValt, 256)
        }
    }

    func readDqtMarker(using self) throw
    {
        left := getBits1(16)
        if left < 2 throw "bad dqt marker"
        left -= 2

        while left
        {
            n    := getBits1(8)
            prec := n >>,safe 4
            n &= 0x0F
            if n >= MaxQuantTables throw "bad dqt table"

            quant[n].resize(64)
            loop i: 64
            {
                temp := getBits1(8)
                if prec
                    temp = (temp << 8) + getBits1(8)
                quant[n].buffer[i] = cast(s16) temp
            }

            i := 64 + 1
            if prec i += 64
            if left < i throw "bad dqt length"
            left -= i
        }
    }

    func readDriMarker(using self) throw
    {
        if getBits1(16) != 4 throw "bad dri length"
        restartInterval = getBits1(16)
    }

    func readSofMarker(using self) throw
    {
        left := getBits1(16)
        if getBits1(8) != 8 throw "bad precision"

        imageSizeY = getBits1(16)
        imageSizeX = getBits1(16)

        if (imageSizeY < 1) || (imageSizeY > MaxHeight) throw "bad height"
        if (imageSizeX < 1) || (imageSizeX > MaxWidth) throw "bad width"

        compsInFrame = getBits1(8)
        if compsInFrame > MaxComponents throw "too many components"
        if left != (compsInFrame * 3 + 8) throw "bad sof length"

        loop i: compsInFrame
        {
            compIdent[i] = getBits1(8)
            compSampH[i] = getBits1(4)
            compSampV[i] = getBits1(4)
            compQuant[i] = getBits1(8)
        }
    }

    func readSosMarker(using self) throw
    {
        left := getBits1(16)
        n    := getBits1(8)
        compsInScan = n
        left -= 3

        if (left != (n * 2 + 3)) || (n < 1) || (n > MaxCompsInScan)
            throw "bad sos length"

        loop i: n
        {
            cc := getBits1(8)
            c  := getBits1(8)
            left -= 2

            ci := 0
            while ci < compsInFrame
            {
                if cc == compIdent[ci]
                    break
                ci += 1
            }

            if ci >= compsInFrame
                throw "bad sos comp id"

            compList[i]   = ci
            compDcTab[ci] = (c >>,safe 4) & 15
            compAcTab[ci] = (c & 15) + (MaxHuffTables >> 1)
        }

        spectralStart  = getBits1(8)
        spectralEnd    = getBits1(8)
        successiveHigh = getBits1(4)
        successiveLow  = getBits1(4)

        if !progressiveFlag
        {
            spectralStart = 0
            spectralEnd   = 63
        }

        left -= 3
        while left
        {
            getBits1(8)
            left -= 1
        }
    }

    func locateSosMarker(using self)->bool throw
    {
        c := try processMarkers()
        if c == .M_EOI
            return false
        else if c != .M_SOS
            throw "unexpected marker"
        try readSosMarker()
        return true
    }

    func locateSoiMarker(using self) throw
    {
        lastRune := getBits1(8)
        thisChar := getBits1(8)
        if (lastRune == 0xFF) && (thisChar == Marker.M_SOI)
            return
        bytesLeft := 512
        while true
        {
            bytesLeft -= 1
            if bytesLeft == 0 throw "not a jpeg"
            lastRune = thisChar
            thisChar = getBits1(8)
            if (lastRune == 0xFF) && (thisChar == Marker.M_SOI)
                break
        }

        thisChar = (bitBuf >> 8) & 0xFF
        if thisChar != 0xFF throw "not a jpeg"
    }

    func locateSofMarker(using self) throw
    {
        try locateSoiMarker()
        c := try processMarkers()
        switch c
        {
        case M_SOF2:
            progressiveFlag = true
            try readSofMarker()
        case M_SOF0, M_SOF1:
            try readSofMarker()
        case M_SOF9:
            throw "no arithmetic support"
        default:
            throw "unsupported marker"
        }
    }

    func memset16(p: *void, c: u16, n: u32)
    {
        ps := cast(*u16) p
        loop n
        {
            :ps = c
            ps += 1
        }
    }

    func prepareInBuffer(using self)
    {
        inBufLeft = 0
        inBufOfs = inBuf
        if eofReached return

        while true
        {
            size := InBufSize - inBufLeft
            if size + streamSeek > streamLen
            {
                size = streamLen - streamSeek
                eofReached = 1
            }

            bytesRead := size
            memory.copy(&inBuf[0] + inBufLeft, stream + streamSeek, cast(uint) size)
            streamSeek += size
            inBufLeft += bytesRead
            if inBufLeft >= InBufSize || eofReached
                break
        }

        totalBytesRead += inBufLeft
        memset16(inBufOfs + inBufLeft, 0xD9FF, 64)
    }
}

public impl IDecoder for Decoder
{
    func canDecode(using self, fileName: string)->bool
    {
        ext := path.getExtensionLowerCase(fileName)
        return ext == ".jpg" || ext == ".jpeg"
    }

    func decode(using self, buffer: const [..] u8)->Image throw
    {
        try
        {
            init(buffer)
            start()

            var result: retval = create(imageSizeX, imageSizeY, PixelFormat.BGR8)
            doit(result.pixels)
        }

        return result
    }
}