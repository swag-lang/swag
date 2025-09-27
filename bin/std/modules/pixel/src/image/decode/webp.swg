#global namespace Webp
using Swag, Core, Core.Errors

public struct Decoder
{
    // Stream data
    stream:        const [*] u8
    streamLen:     s32

    // Image properties
    imageSizeX:     s32
    imageSizeY:     s32

    // Decoder options and metadata
    decOptions:     DecodeOptions
    metaDatas:      Array'ImageMetaData
}

private
{
    // VP8 coefficient token values
    const DCT_0    = 0
    const DCT_1    = 1
    const DCT_2    = 2
    const DCT_3    = 3
    const DCT_4    = 4
    const DCT_CAT1 = 5
    const DCT_CAT2 = 6
    const DCT_CAT3 = 7
    const DCT_CAT4 = 8
    const DCT_CAT5 = 9
    const DCT_CAT6 = 10

    const MaxSize = 1_000_000_000
    const MaxDim  = 8192
    const MBSize  = 16
    const BLK4    = 4

    struct VP8Header
    {
        keyFrame:        bool
        version:         s32
        showFrame:       bool
        firstPartSz:     s32
        width:           s32
        height:          s32
        scaleW:          u8
        scaleH:          u8
        yStride:         s32
        uvStride:        s32
    }

    // VP8 Default coefficient probabilities
    // Format: [plane_type][band][context][prob_index]
    const CoeffProbsDefault = [
        // Y plane
        [
            [[255, 255, 255], [255, 255, 255], [255, 255, 255]], // Band 0
            [[145, 199, 177], [67, 101, 66], [64, 90, 47]], // Band 1
            [[187, 224, 203], [77, 123, 85], [68, 97, 55]], // Band 2
            [[214, 235, 211], [85, 130, 90], [71, 98, 57]], // Band 3
            [[229, 240, 215], [87, 131, 92], [72, 100, 58]], // Band 4
            [[237, 243, 218], [88, 132, 93], [73, 101, 60]]],
        // UV plane
        [
            [[255, 255, 255], [255, 255, 255], [255, 255, 255]], // Band 0
            [[156, 203, 179], [70, 118, 71], [67, 94, 50]], // Band 1
            [[189, 223, 201], [75, 123, 83], [69, 97, 54]], // Band 2
            [[214, 235, 210], [83, 129, 89], [70, 98, 56]], // Band 3
            [[228, 241, 214], [85, 131, 91], [71, 100, 58]], // Band 4
            [[236, 244, 217], [87, 132, 92], [72, 101, 59]]]]

    // VP8 zigzag scan order and band mapping
    const ZigZag4x4:  [16] u8 = [0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15]
    const BandForPos: [16] u8 = [0, 1, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5]

    // Basic quantization tables
    const QuantDC: [2] s32 = [6, 5]     // Y DC, UV DC
    const QuantAC: [2] s32 = [4, 4]     // Y AC, UV AC

    // Boolean decoder
    struct BoolDec
    {
        range:      u32
        value:      u32
        bitpos:     s32
        p:          const [*] u8
        end:        const [*] u8
    }

    // --- Transform functions ---

    // VP8 4x4 IDCT
    func idct4(src: const [*] s16, dst: [*] s16)
    {
        var temp: [16] s32

        func clampPixel(v: s32)->s16
        {
            return cast(s16) Math.clamp(v, 0, 255)
        }

        // First pass: columns (vertical transform)
        for i in 4
        {
            let ip  = cast(s32) src[i + 0]
            let ip1 = cast(s32) src[i + 4]
            let ip2 = cast(s32) src[i + 8]
            let ip3 = cast(s32) src[i + 12]

            let a1    = ip + ip3
            let b1    = ip - ip3
            let temp1 = (ip2 * 35468) >> 16
            let temp2 = ip1 + temp1
            let temp3 = ip1 * 20091
            let temp4 = (temp3 + (ip2 * 52808)) >> 16
            let c1    = temp2 - temp4

            temp[i + 0]  = a1 + temp4
            temp[i + 4]  = b1 + c1
            temp[i + 8]  = b1 - c1
            temp[i + 12] = a1 - temp4
        }

        // Second pass: rows (horizontal transform)
        foreach i in [0, 4, 8, 12]
        {
            let a1    = temp[i + 0] + temp[i + 3]
            let b1    = temp[i + 0] - temp[i + 3]
            let temp1 = (temp[i + 2] * 35468) >> 16
            let temp2 = temp[i + 1] + temp1
            let temp3 = temp[i + 1] * 20091
            let temp4 = (temp3 + (temp[i + 2] * 52808)) >> 16
            let c1    = temp2 - temp4

            dst[i + 0] = clampPixel((a1 + temp4 + 4) >> 3)
            dst[i + 1] = clampPixel((b1 + c1 + 4) >> 3)
            dst[i + 2] = clampPixel((b1 - c1 + 4) >> 3)
            dst[i + 3] = clampPixel((a1 - temp4 + 4) >> 3)
        }
    }

    // --- Boolean decoder functions ---

    func initBool(b: *BoolDec, src: const [*] u8, len: s32)
    {
        b.range  = 255
        b.bitpos = 0
        b.p      = src
        b.end    = src + len

        if len >= 3
        {
            b.value = (cast(u32) src[0] << 16) | (cast(u32) src[1] << 8) | cast(u32) src[2]
            b.p     = src + 3
        }
        elif len == 2
        {
            b.value = (cast(u32) src[0] << 16) | (cast(u32) src[1] << 8)
            b.p     = src + 2
        }
        elif len == 1
        {
            b.value = (cast(u32) src[0] << 16)
            b.p     = src + 1
        }
        else
        {
            b.value = 0
        }
    }

    func readBool(b: *BoolDec, prob: u32)->u32
    {
        let p     = Math.min(prob, 255)
        let split = 1 + (((b.range - 1) * p) >> 8)
        let big   = b.value >= (cast(u32) split << 8)

        if big
        {
            b.value -= cast(u32) split << 8
            b.range -= split
        }
        else
        {
            b.range = split
        }

        // Normalize
        while b.range < 128
        {
            b.range <<= 1
            b.value <<= 1
            b.bitpos += 1

            if b.bitpos == 8
            {
                b.bitpos = 0
                if b.p < b.end
                {
                    b.value |= cast(u32) (dref b.p)
                    b.p += 1
                }
            }
        }

        return big ? 1 : 0
    }

    func readBit(b: *BoolDec)->u32 => readBool(b, 128)

    // --- Coefficient decoding ---

    func decodeBlock4x4(b: *BoolDec, coeffs: [*] s16, plane: s32)->bool
    {
        // Clear coefficients
        Memory.set(coeffs, 0, #sizeof(s16) * 16)

        let bt        = (plane == 0) ? 0 : 1
        var hasCoeffs = false

        for i in 16
        {
            let pos      = ZigZag4x4[i]
            let band     = cast(s32) BandForPos[pos]
            let safeBand = Math.min(band, 5)

            // Simple context determination
            var ctx = 0
            if i > 0 and coeffs[ZigZag4x4[i - 1]] != 0
            {
                ctx = 1
            }
            elif i > 1 and coeffs[ZigZag4x4[i - 2]] != 0
            {
                ctx = 1
            }
            ctx = Math.min(ctx, 2)

            // Check for end of block
            if readBool(b, CoeffProbsDefault[bt][safeBand][ctx][0]) == 0:
                break

            // Check if coefficient is non-zero
            if readBool(b, CoeffProbsDefault[bt][safeBand][ctx][1]) == 0:
                continue

            hasCoeffs = true

            // Decode coefficient magnitude
            var level = 1
            if readBool(b, CoeffProbsDefault[bt][safeBand][ctx][2]) != 0
            {
                level = 2
                while readBit(b) != 0 and level < 40
                {
                    level += 1
                }
            }

            // Add sign
            if readBit(b) != 0:
                level = -level

            coeffs[pos] = cast(s16) level
        }

        return hasCoeffs
    }

    // Dequantize coefficients
    func dequantizeBlock(coeffs: [*] s16, plane: s32, isDC: bool)
    {
        let qp = isDC ? QuantDC[plane] : QuantAC[plane]

        for i in 16
        {
            if coeffs[i] != 0
            {
                coeffs[i] = cast(s16) (cast(s32) coeffs[i] * qp)
            }
        }
    }

    // --- YUV to BGR conversion ---

    func yuvToBgrRow(dst: [*] u8, y: const [*] s16, u: const [*] s16, v: const [*] s16, width: s32)
    {
        func sr8(x: s32)->s32 => (x + 128) >> 8

        func convertPixel(Y: s32, Uc: s32, Vc: s32)->{ r: s32, g: s32, b: s32 }
        {
            let C = Math.max(Y - 16, 0)
            let r = sr8(298 * C + 409 * Vc)
            let g = sr8(298 * C - 100 * Uc - 208 * Vc)
            let b = sr8(298 * C + 516 * Uc)

            return {
                Math.clamp(r, 0, 255),
                Math.clamp(g, 0, 255),
                Math.clamp(b, 0, 255)}
        }

        var x = 0
        while x + 1 < width
        {
            let Y0 = cast(s32) y[x + 0]
            let Y1 = cast(s32) y[x + 1]
            let Uc = cast(s32) u[x >> 1] - 128
            let Vc = cast(s32) v[x >> 1] - 128

            let (r0, g0, b0) = convertPixel(Y0, Uc, Vc)
            let (r1, g1, b1) = convertPixel(Y1, Uc, Vc)

            let offset = x * 3
            dst[offset + 0] = cast(u8) b0
            dst[offset + 1] = cast(u8) g0
            dst[offset + 2] = cast(u8) r0
            dst[offset + 3] = cast(u8) b1
            dst[offset + 4] = cast(u8) g1
            dst[offset + 5] = cast(u8) r1

            x += 2
        }

        // Handle odd width
        if x < width
        {
            let Y  = cast(s32) y[x]
            let Uc = cast(s32) u[x >> 1] - 128
            let Vc = cast(s32) v[x >> 1] - 128
            let (r, g, b) = convertPixel(Y, Uc, Vc)

            let offset = x * 3
            dst[offset + 0] = cast(u8) b
            dst[offset + 1] = cast(u8) g
            dst[offset + 2] = cast(u8) r
        }
    }

    // --- Partition management ---

    struct TokenPartitions
    {
        decoders:     [8] BoolDec
        count:        s32
    }

    func initTokenPartitions(tp: *TokenPartitions, data: const [*] u8, size: s32, tokensStart: s32)
    {
        func readSize24(p: const [*] u8)->s32 => cast(s32) p[0] | (cast(s32) p[1] << 8) | (cast(s32) p[2] << 16)

        let tokensAvail = size - tokensStart
        var partOfs:    [8] s32
        var partLen:    [8] s32

        // Try different partition counts (1, 2, 4, 8)
        const candidates: [4] s32 = [1, 2, 4, 8]
        tp.count = 1 // default fallback

        foreach numParts in candidates
        {
            let hdrSize = 3 * (numParts - 1)
            if hdrSize > tokensAvail:
                continue

            var valid   = true
            var sizeSum = 0

            // Calculate partition sizes
            for i in numParts - 1
            {
                let s = readSize24(data + tokensStart + (i * 3))
                if s < 0 or s > tokensAvail
                {
                    valid = false
                    break
                }
                partLen[i] = s
                sizeSum += s
            }

            if !valid:
                continue

            let tailSize = tokensAvail - hdrSize - sizeSum
            if tailSize < 0:
                continue

            // Set up partition offsets and sizes
            partOfs[0] = cast(s32) (tokensStart + hdrSize)
            for i in 1 until numParts
            {
                partOfs[i] = partOfs[i - 1] + partLen[i - 1]
            }

            if numParts == 1
            {
                partOfs[0] = tokensStart
                partLen[0] = tokensAvail
            }
            else
            {
                partLen[numParts - 1] = cast(s32) tailSize
            }

            // Validate all partitions
            valid = true
            for i in numParts
            {
                if partLen[i] <= 0 or partOfs[i] < tokensStart or
                   (partOfs[i] + partLen[i]) > (tokensStart + tokensAvail)
                {
                    valid = false
                    break
                }
            }

            if valid
            {
                tp.count = cast() numParts
                break
            }
        }

        // Initialize decoders
        for i in tp.count
        {
            initBool(&tp.decoders[i], data + partOfs[i], partLen[i])
        }
    }

    func getPartitionForMB(tp: *TokenPartitions, mbRow: s32)->*BoolDec
    {
        if tp.count == 1:
            return &tp.decoders[0]
        let idx = mbRow & (tp.count - 1)
        return &tp.decoders[idx]
    }
}

impl Decoder
{
    // --- Helper functions ---

    mtd dU8(at: s32)->u8 => (at < 0 or at >= me.streamLen) ? 0 : me.stream[at]

    mtd dU16(at: s32)->u16 => cast(u16) me.dU8(at) | cast(u16) me.dU8(at + 1) << 8

    mtd dU32(at: s32)->u32 => cast(u32) me.dU8(at) | (cast(u32) me.dU8(at + 1) << 8) |
                              (cast(u32) me.dU8(at + 2) << 16) | (cast(u32) me.dU8(at + 3) << 24)

    mtd isTag(at: s32, a: u8, b: u8, c: u8, d: u8)->bool => me.dU8(at + 0) == a and me.dU8(at + 1) == b and
                                                            me.dU8(at + 2) == c and me.dU8(at + 3) == d

    // --- RIFF parsing ---

    mtd parseRiff()->{ offset, size: s32 } throw
    {
        if me.streamLen < 12:
            throw InvalidFormatError{"webp: too short"}

        if !(me.isTag(0, 'R', 'I', 'F', 'F') and me.isTag(8, 'W', 'E', 'B', 'P')):
            throw InvalidFormatError{"webp: missing RIFF/WEBP header"}

        let riffSize = cast(s32) me.dU32(4) + 8
        if riffSize > me.streamLen or riffSize <= 0 or riffSize > MaxSize:
            throw InvalidFormatError{"webp: bad RIFF size"}

        return me.findVP8Chunk()
    }

    mtd findVP8Chunk()->{ offset, size: s32 } throw
    {
        var p = 12
        while (p + 8) <= me.streamLen
        {
            let tag        = me.dU32(p + 0)
            let chunkSize  = cast(s32) me.dU32(p + 4)
            let dataOffset = p + 8

            if tag == (cast(u32) 'V' | (cast(u32) 'P' << 8) |
                      (cast(u32) '8' << 16) | (cast(u32) ' ' << 24))
            {
                let actualSize = Math.min(chunkSize, me.streamLen - dataOffset)
                return {dataOffset, actualSize}
            }

            p = dataOffset + ((chunkSize + 1) & ~1) // chunks are padded to even
        }

        throw InvalidFormatError{"webp: VP8 missing"}
        return {0, 0}
    }

    // --- VP8 frame parsing ---

    mtd parseVP8Frame(ofs: s32, size: s32)->VP8Header throw
    {
        if size < 10:
            throw InvalidFormatError{"vp8: chunk too short"}

        let b0 = me.dU8(ofs + 0)
        let b1 = me.dU8(ofs + 1)
        let b2 = me.dU8(ofs + 2)

        var hdr: VP8Header
        hdr.keyFrame    = (b0 & 1) == 0
        hdr.version     = (b0 >> 1) & 7
        hdr.showFrame   = ((b0 >> 4) & 1) == 1
        hdr.firstPartSz = cast(s32) (b0 >> 5) | (cast(s32) b1 << 3) | (cast(s32) b2 << 11)

        if !hdr.keyFrame:
            throw UnsupportedFormatError{"vp8: only keyframes supported"}

        if !(me.dU8(ofs + 3) == 0x9D and me.dU8(ofs + 4) == 0x01 and me.dU8(ofs + 5) == 0x2A):
            throw InvalidFormatError{"vp8: invalid keyframe signature"}

        hdr.width  = cast(s32) (me.dU16(ofs + 6) & 0x3FFF)
        hdr.height = cast(s32) (me.dU16(ofs + 8) & 0x3FFF)
        hdr.scaleW = cast(u8) ((me.dU16(ofs + 6) >> 14) & 3)
        hdr.scaleH = cast(u8) ((me.dU16(ofs + 8) >> 14) & 3)

        if hdr.width <= 0 or hdr.height <= 0 or hdr.width > MaxDim or hdr.height > MaxDim:
            throw InvalidFormatError{"vp8: invalid dimensions"}

        hdr.yStride  = hdr.width
        hdr.uvStride = (hdr.width + 1) >> 1

        me.imageSizeX = hdr.width
        me.imageSizeY = hdr.height

        return hdr
    }

    // --- Block reconstruction ---

    func reconstructBlock(coeffs: [*] s16, recon: [*] s16, hasCoeffs: bool, dcValue: s32)
    {
        if hasCoeffs
        {
            idct4(coeffs, recon)
        }
        else
        {
            Memory.set(recon, 0, #sizeof(s16) * 16)
        }

        // Apply DC prediction
        for i in 16
        {
            let val = dcValue + cast(s32) recon[i]
            recon[i] = cast(s16) Math.clamp(val, 0, 255)
        }
    }

    func calculateDCPredictor(plane: [*] s16, stride: s32, x: s32, y: s32, blockSize: s32)->s32
    {
        var sum   = 0
        var count = 0

        // Top neighbors
        if y > 0
        {
            for i in blockSize
            {
                if (x + i) < stride
                {
                    sum += cast(s32) plane[(y - 1) * stride + x + i]
                    count += 1
                }
            }
        }

        // Left neighbors
        if x > 0
        {
            for i in blockSize
            {
                sum += cast(s32) plane[(y + i) * stride + (x - 1)]
                count += 1
            }
        }

        return count > 0 ? sum / count : 128
    }

    // --- Main decode function ---

    mtd decodeKeyframe(ofs: s32, size: s32, yPlane: [*] s16, uPlane: [*] s16, vPlane: [*] s16) throw
    {
        let hdr = me.parseVP8Frame(ofs, size)

        let part0Start = ofs + 10
        let part0Size  = hdr.firstPartSz

        if part0Size <= 0 or (part0Start + part0Size) > (ofs + size):
            throw InvalidFormatError{"vp8: invalid partition 0 size"}

        // Parse minimal header
        var headerDec: BoolDec
        initBool(&headerDec, me.stream + part0Start, part0Size)
        discard readBit(&headerDec) // color_space
        discard readBit(&headerDec) // clamping_type

        // Initialize token partitions
        var tokenPartitions: TokenPartitions
        initTokenPartitions(&tokenPartitions, me.stream, ofs + size, part0Start + part0Size)

        decodeMacroblocks(hdr, &tokenPartitions, yPlane, uPlane, vPlane)
    }

    func decodeMacroblocks(hdr: VP8Header, tp: *TokenPartitions, yPlane: [*] s16, uPlane: [*] s16, vPlane: [*] s16)
    {
        let mbW = (hdr.width + 15) >> 4
        let mbH = (hdr.height + 15) >> 4

        for mbY in mbH
        {
            let tokenDec = getPartitionForMB(tp, mbY)

            for mbX in mbW
            {
                decodeMacroblock(mbX, mbY, hdr, tokenDec, yPlane, uPlane, vPlane)
            }
        }
    }

    func decodeMacroblock(mbX: s32, mbY: s32, hdr: VP8Header, tokenDec: *BoolDec, yPlane: [*] s16, uPlane: [*] s16, vPlane: [*] s16)
    {
        var coeffs: [16] s16
        var recon:  [16] s16

        // Decode Y blocks (16 4x4 blocks)
        for by in 4
        {
            for bx in 4
            {
                let hasCoeffs = decodeBlock4x4(tokenDec, &coeffs[0], 0)
                if hasCoeffs:
                    dequantizeBlock(&coeffs[0], 0, false)

                let blockX  = mbX * 16 + bx * 4
                let blockY  = mbY * 16 + by * 4
                let dcValue = calculateDCPredictor(yPlane, hdr.yStride, blockX, blockY, 4)

                reconstructBlock(&coeffs[0], &recon[0], hasCoeffs, dcValue)
                copyBlockToPlane(&recon[0], yPlane, hdr.yStride, blockX, blockY, hdr.width, hdr.height)
            }
        }

        // Decode U and V blocks (4 4x4 blocks each)
        decodeChromaBlocks(mbX, mbY, hdr, tokenDec, uPlane, vPlane)
    }

    func decodeChromaBlocks(mbX: s32, mbY: s32, hdr: VP8Header, tokenDec: *BoolDec, uPlane: [*] s16, vPlane: [*] s16)
    {
        let chromaW = (hdr.width + 1) >> 1
        let chromaH = (hdr.height + 1) >> 1
        var coeffs: [16] s16
        var recon:  [16] s16

        // Decode U blocks
        for cby in 2
        {
            for cbx in 2
            {
                for sub in 4
                {
                    let hasCoeffs = decodeBlock4x4(tokenDec, &coeffs[0], 1)
                    if hasCoeffs:
                        dequantizeBlock(&coeffs[0], 1, false)

                    let blockX  = mbX * 8 + cbx * 4 + ((sub & 1) * 4)
                    let blockY  = mbY * 8 + cby * 4 + ((sub >> 1) * 4)
                    let dcValue = calculateDCPredictor(uPlane, hdr.uvStride, blockX, blockY, 4)

                    reconstructBlock(&coeffs[0], &recon[0], hasCoeffs, dcValue)
                    copyBlockToPlane(&recon[0], uPlane, hdr.uvStride, blockX, blockY, chromaW, chromaH)
                }
            }
        }

        // Decode V blocks
        for cby in 2
        {
            for cbx in 2
            {
                for sub in 4
                {
                    let hasCoeffs = decodeBlock4x4(tokenDec, &coeffs[0], 1)
                    if hasCoeffs:
                        dequantizeBlock(&coeffs[0], 1, false)

                    let blockX  = mbX * 8 + cbx * 4 + ((sub & 1) * 4)
                    let blockY  = mbY * 8 + cby * 4 + ((sub >> 1) * 4)
                    let dcValue = calculateDCPredictor(vPlane, hdr.uvStride, blockX, blockY, 4)

                    reconstructBlock(&coeffs[0], &recon[0], hasCoeffs, dcValue)
                    copyBlockToPlane(&recon[0], vPlane, hdr.uvStride, blockX, blockY, chromaW, chromaH)
                }
            }
        }
    }

    func copyBlockToPlane(block: [*] s16, plane: [*] s16, stride: s32, startX: s32, startY: s32, maxW: s32, maxH: s32)
    {
        for py in 4
        {
            for px in 4
            {
                let x = startX + px
                let y = startY + py
                if x < maxW and y < maxH
                {
                    plane[y * stride + x] = block[py * 4 + px]
                }
            }
        }
    }

    // --- Image conversion ---

    mtd convertToRGB(pixels: [*] u8, yPlane: [*] s16, uPlane: [*] s16, vPlane: [*] s16)
    {
        let width  = me.imageSizeX
        let height = me.imageSizeY

        for y in height
        {
            let yRow   = yPlane + y * width
            let uRow   = uPlane + (y >> 1) * ((width + 1) >> 1)
            let vRow   = vPlane + (y >> 1) * ((width + 1) >> 1)
            let dstRow = pixels + y * width * 3
            yuvToBgrRow(dstRow, yRow, uRow, vRow, width)
        }
    }

    // --- Main interface methods ---

    mtd start() throw
    {
        let (?, vp8Size) = me.parseRiff()
        if vp8Size <= 0:
            throw InvalidFormatError{"webp/vp8: zero size"}
    }

    mtd doit(pixels: [*] u8) throw
    {
        let (vp8Offset, vp8Size) = me.findVP8Chunk()

        // Allocate YUV planes
        let width   = me.imageSizeX
        let height  = me.imageSizeY
        let chromaW = (width + 1) >> 1
        let chromaH = (height + 1) >> 1

        var yPlane: Array's16
        var uPlane: Array's16
        var vPlane: Array's16

        yPlane.resize(width * height)
        uPlane.resize(chromaW * chromaH)
        vPlane.resize(chromaW * chromaH)

        // Initialize chroma planes to neutral value
        for i in @countof(uPlane)
        {
            uPlane[i] = 128
            vPlane[i] = 128
        }

        // Decode VP8 keyframe
        me.decodeKeyframe(vp8Offset, vp8Size, @dataof(yPlane), @dataof(uPlane), @dataof(vPlane))

        // Convert YUV to BGR
        me.convertToRGB(pixels, @dataof(yPlane), @dataof(uPlane), @dataof(vPlane))
    }

    mtd init(buffer: const [..] u8) throw
    {
        me.stream    = @dataof(buffer)
        me.streamLen = cast(s32) @countof(buffer)

        let (vp8Offset, vp8Size) = me.parseRiff()
        if vp8Offset < 0 or vp8Size <= 0:
            throw InvalidFormatError{"webp: no VP8 chunk"}

        let hdr = me.parseVP8Frame(vp8Offset, vp8Size)
        me.imageSizeX = hdr.width
        me.imageSizeY = hdr.height
    }
}

public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        let ext = Path.getExtensionLowerCase(fileName)
        return ext == ".webp"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        .decOptions = options
        .metaDatas.clear()
        .init(buffer)
        .start()

        var result: Image
        result.init(.imageSizeX, .imageSizeY, PixelFormat.BGR8)

        if !options.decodePixels:
            return result

        result.allocPixels()
        .doit(result.pixels.buffer)
        result.metaDatas = #move .metaDatas

        return result
    }
}
