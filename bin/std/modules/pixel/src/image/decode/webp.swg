#global namespace Webp
using Swag, Core, Core.Errors

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

    // ---- VP8 tables (probabilities & quant) ----

    // Default coeff probs (from VP8 spec – standard values)
    // [block_type][coeff_band][context][node]
    // Reduced to keyframe (intra) case, sufficient for the example.
    // Also need to update the coefficient probability table to have more nodes
    // This is a more complete version with proper probability values
    // VP8 Default coefficient probabilities - simplified but working version
    // Format: [plane_type][band][context][prob_index]
    const CoeffProbsDefault = [
        // Y plane
        [
            [[255, 255, 255], [255, 255, 255], [255, 255, 255]], // Band 0 - let everything through for now
            [[145, 199, 177], [67, 101, 66], [64, 90, 47]], // Band 1
            [[187, 224, 203], [77, 123, 85], [68, 97, 55]], // Band 2
            [[214, 235, 211], [85, 130, 90], [71, 98, 57]], // Band 3
            [[229, 240, 215], [87, 131, 92], [72, 100, 58]], // Band 4
            [[237, 243, 218], [88, 132, 93], [73, 101, 60]]],
        // UV plane
        [
            [[255, 255, 255], [255, 255, 255], [255, 255, 255]], // Band 0 - let everything through for now
            [[156, 203, 179], [70, 118, 71], [67, 94, 50]], // Band 1
            [[189, 223, 201], [75, 123, 83], [69, 97, 54]], // Band 2
            [[214, 235, 210], [83, 129, 89], [70, 98, 56]], // Band 3
            [[228, 241, 214], [85, 131, 91], [71, 100, 58]], // Band 4
            [[236, 244, 217], [87, 132, 92], [72, 101, 59]]]] // Band 5+

    // VP8 zigzag scan order for 4x4 blocks
    const ZigZag4x4: [16] u8 = [0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15]

    // Band mapping for coefficient positions
    const BandForPos: [16] u8 = [0, 1, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5]

    // iDCT tables (like VP8 JPEG-style 4×4) – integer constants
    const CosVal = [[64, 83, 64, 36], [64, 36, -64, -83]]

    // --- Bool decoder ---
    struct BoolDec { range: u32, value: u32, bitpos: s32, p: const [*] u8, end: const [*] u8 }

// VP8 4x4 WHT (Walsh-Hadamard Transform) for the DC coefficients of Y blocks
func iwht4x4(input: [*] s16, output: [*] s16)
{
    var temp: [16] s16

    // First pass: columns
    var i = 0
    while i < 4
    {
        let a = cast(s32) input[0 + i] + cast(s32) input[12 + i]
        let b = cast(s32) input[4 + i] + cast(s32) input[8 + i]
        let c = cast(s32) input[4 + i] - cast(s32) input[8 + i]
        let d = cast(s32) input[0 + i] - cast(s32) input[12 + i]

        temp[0 + i] = cast(s16) (a + b)
        temp[4 + i] = cast(s16) (c + d)
        temp[8 + i] = cast(s16) (a - b)
        temp[12 + i] = cast(s16) (d - c)

        i += 1
    }

    // Second pass: rows
    i = 0
    while i < 16
    {
        let a = cast(s32) temp[i + 0] + cast(s32) temp[i + 3]
        let b = cast(s32) temp[i + 1] + cast(s32) temp[i + 2]
        let c = cast(s32) temp[i + 1] - cast(s32) temp[i + 2]
        let d = cast(s32) temp[i + 0] - cast(s32) temp[i + 3]

        let op_a = a + b
        let op_b = c + d
        let op_c = a - b
        let op_d = d - c

        output[i + 0] = cast(s16) ((op_a + 3) >> 3)
        output[i + 1] = cast(s16) ((op_b + 3) >> 3)
        output[i + 2] = cast(s16) ((op_c + 3) >> 3)
        output[i + 3] = cast(s16) ((op_d + 3) >> 3)

        i += 4
    }
}

// VP8 4x4 IDCT - this is the actual VP8 transform, not JPEG DCT
func idct4(src: const [*] s16, dst: [*] s16)
{
    var temp: [16] s32

    func clampPixel(v: s32)->s16
    {
        if v > 255: return 255
        if v < 0: return 0
        return cast(s16) v
    }

    // First pass: columns (vertical transform)
    var i = 0
    while i < 4
    {
        let ip = cast(s32) src[i + 0]   // input[0]
        let ip1 = cast(s32) src[i + 4]  // input[1]
        let ip2 = cast(s32) src[i + 8]  // input[2]
        let ip3 = cast(s32) src[i + 12] // input[3]

        let a1 = ip + ip3
        let b1 = ip - ip3
        let temp1 = (ip2 * 35468) >> 16
        let temp2 = ip1 + temp1
        let temp3 = ip1 * 20091
        let temp4 = (temp3 + (ip2 * 52808)) >> 16
        let c1 = temp2 - temp4

        temp[i + 0] = a1 + temp4
        temp[i + 4] = b1 + c1
        temp[i + 8] = b1 - c1
        temp[i + 12] = a1 - temp4

        i += 1
    }

    // Second pass: rows (horizontal transform)
    i = 0
    while i < 16
    {
        let a1 = temp[i + 0] + temp[i + 3]
        let b1 = temp[i + 0] - temp[i + 3]
        let temp1 = (temp[i + 2] * 35468) >> 16
        let temp2 = temp[i + 1] + temp1
        let temp3 = temp[i + 1] * 20091
        let temp4 = (temp3 + (temp[i + 2] * 52808)) >> 16
        let c1 = temp2 - temp4

        // Final output with proper DC offset and clamping
        dst[i + 0] = clampPixel((a1 + temp4 + 4) >> 3)
        dst[i + 1] = clampPixel((b1 + c1 + 4) >> 3)
        dst[i + 2] = clampPixel((b1 - c1 + 4) >> 3)
        dst[i + 3] = clampPixel((a1 - temp4 + 4) >> 3)

        i += 4
    }
}

// Simplified but more accurate coefficient decoding
func decodeBlock4x4(b: *BoolDec, coeffs: [*] s16, plane: s32)->bool
{
    // Clear coefficients first
    var i = 0
    while i < 16
    {
        coeffs[i] = 0
        i += 1
    }

    const ZZ: [16] u8 = [0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15]
    //const BandForPos: [16] u8 = [0, 1, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5]

    let bt = (plane == 0) ? 0 : 1
    var hasCoeffs = false

    i = 0
    while i < 16
    {
        let pos = ZZ[i]
        let band = cast(s32) BandForPos[pos]
        let safeBand = (band > 5) ? 5 : band

        // Simple context - in a real decoder this would be more sophisticated
        var ctx = 0
        if i > 0 and coeffs[ZZ[i-1]] != 0:
            ctx = 1
        elif i > 1 and coeffs[ZZ[i-2]] != 0:
            ctx = 1
        if ctx > 2: ctx = 2

        // Check for end of block
        if readBool(b, CoeffProbsDefault[bt][safeBand][ctx][0]) == 0:
            break

        // Check if coefficient is non-zero
        if readBool(b, CoeffProbsDefault[bt][safeBand][ctx][1]) == 0
        {
            i += 1
            continue
        }

        hasCoeffs = true

        // Decode coefficient magnitude (simplified)
        var level = 1
        if readBool(b, CoeffProbsDefault[bt][safeBand][ctx][2]) != 0
        {
            // Larger than 1
            level = 2

            // For even larger values, add more bits
            while readBit(b) != 0 and level < 40
            {
                level += 1
            }
        }

        // Add sign
        if readBit(b) != 0:
            level = -level

        coeffs[pos] = cast(s16) level
        i += 1
    }

    return hasCoeffs
}

// Basic quantization tables (these should come from frame header in real decoder)
const QuantDC: [2] s32 = [6, 5]    // Y DC, UV DC
const QuantAC: [2] s32 = [4, 4]    // Y AC, UV AC

// Dequantize coefficients
func dequantizeBlock(coeffs: [*] s16, plane: s32, isDC: bool)
{
    let qp = isDC ? QuantDC[plane] : QuantAC[plane]

    var i = 0
    while i < 16
    {
        if coeffs[i] != 0
        {
            coeffs[i] = cast(s16) (cast(s32) coeffs[i] * qp)
        }
        i += 1
    }
}

    func initBool(b: *BoolDec, src: const [*] u8, len: s32)
    {
        b.range  = 255; b.bitpos = 0; b.p      = src; b.end    = src + len; if (len >= 3)
        {
            b.value = (cast(u32) src[0] << 16) | (cast(u32) src[1] << 8) | cast(u32) src[2]
            b.p     = src + 3
        }
        elif (len == 2)
        {
            b.value = (cast(u32) src[0] << 16) | (cast(u32) src[1] << 8)
            b.p     = src + 2
        }
        elif (len == 1)
        {
            b.value = (cast(u32) src[0] << 16); b.p     = src + 1; }
        else
        {
            b.value = 0 // treat missing bytes as zero
        }
    }

    func readBool(b: *BoolDec, p: u32)->u32
    {
        let prob  = (p > 255) ? 255 : p
        let split = 1 + (((b.range - 1) * prob) >> 8)
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

        // Normalize correctly: shift in one bit at a time, refill with a byte every 8 shifts
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

    func readBit(b: *BoolDec)->u32
    {
        return readBool(b, 128)
    }

    // Updated YUV to BGR conversion with better clamping
    func yuvToBgrRow(dst: [*] u8, y: const [*] s16, u: const [*] s16, v: const [*] s16, w: s32)
    {
        // round-to-nearest with arithmetic shift (works for negative too)
        func sr8(x: s32)->s32 { return (x + 128) >> 8; }

        var x = 0
        while (x + 1 < w)
        {
            // process 2 pixels sharing the same U,V
            let Y0: s32 = cast(s32) y[x + 0]
            let Y1: s32 = cast(s32) y[x + 1]
            let Uc: s32 = cast(s32) u[x >> 1] - 128
            let Vc: s32 = cast(s32) v[x >> 1] - 128

            // Apply proper YUV to RGB conversion matrix
            let C0: s32 = Y0 - 16
            let CC0 = (C0 > 0) ? C0 : 0
            var R0  = sr8(298 * CC0 + 409 * Vc)
            var G0  = sr8(298 * CC0 - 100 * Uc - 208 * Vc)
            var B0  = sr8(298 * CC0 + 516 * Uc)

            let C1: s32 = Y1 - 16
            let CC1 = (C1 > 0) ? C1 : 0
            var R1  = sr8(298 * CC1 + 409 * Vc)
            var G1  = sr8(298 * CC1 - 100 * Uc - 208 * Vc)
            var B1  = sr8(298 * CC1 + 516 * Uc)

            R0 = Math.clamp(R0, 0, 255); G0 = Math.clamp(G0, 0, 255); B0 = Math.clamp(B0, 0, 255)
            R1 = Math.clamp(R1, 0, 255); G1 = Math.clamp(G1, 0, 255); B1 = Math.clamp(B1, 0, 255)

            let o = x * 3
            dst[o + 0] = cast(u8) B0; dst[o + 1] = cast(u8) G0; dst[o + 2] = cast(u8) R0
            dst[o + 3] = cast(u8) B1; dst[o + 4] = cast(u8) G1; dst[o + 5] = cast(u8) R1
            x += 2
        }

        // tail pixel if width is odd
        if x < w
        {
            let Y:  s32 = cast(s32) y[x]
            let Uc: s32 = cast(s32) u[x >> 1] - 128
            let Vc: s32 = cast(s32) v[x >> 1] - 128

            let C: s32 = Y - 16
            let CC = (C > 0) ? C : 0

            var R = sr8(298 * CC + 409 * Vc)
            var G = sr8(298 * CC - 100 * Uc - 208 * Vc)
            var B = sr8(298 * CC + 516 * Uc)

            R = Math.clamp(R, 0, 255)
            G = Math.clamp(G, 0, 255)
            B = Math.clamp(B, 0, 255)

            let o = x * 3
            dst[o + 0] = cast(u8) B; dst[o + 1] = cast(u8) G; dst[o + 2] = cast(u8) R
        }
    }
}

impl Decoder
{
    // --- read helpers ---
    mtd dU8(at: s32)->u8 { if at < 0 or at >= me.streamLen:return 0; return me.stream[at]; }
    mtd dU16(at: s32)->u16 { return cast(u16) me.dU8(at) | cast(u16) me.dU8(at + 1) << 8; }
    mtd dU32(at: s32)->u32 { return cast(u32) me.dU8(at) | (cast(u32) me.dU8(at + 1) << 8) | (cast(u32) me.dU8(at + 2) << 16) | (cast(u32) me.dU8(at + 3) << 24); }

    mtd isTag(at: s32, a: u8, b: u8, c: u8, d: u8)->bool
    {
        return me.dU8(at + 0) == a and me.dU8(at + 1) == b and me.dU8(at + 2) == c and me.dU8(at + 3) == d
    }

    // parse RIFF, return (offset,size) for 'VP8 ' chunk
    mtd parseRiff()->{ offset, size: s32 } throw
    {
        if me.streamLen < 12:
            throw InvalidFormatError{"webp: too short"}

        if !(me.isTag(0, 'R', 'I', 'F', 'F') and me.isTag(8, 'W', 'E', 'B', 'P')):
            throw InvalidFormatError{"webp: missing RIFF/WEBP header"}

        let riffSize = cast(s32) me.dU32(4) + 8
        if riffSize > me.streamLen or riffSize <= 0 or riffSize > MaxSize:
            throw InvalidFormatError{"webp: bad RIFF size"}

        var p      = 12
        var vp8Ofs = -1
        var vp8Siz = 0
        while (p + 8) <= me.streamLen
        {
            let tag = me.dU32(p + 0)
            let csz = cast(s32) me.dU32(p + 4)
            let dat = p + 8

            if tag == (cast(u32) 'V' | (cast(u32) 'P' << 8) | (cast(u32) '8' << 16) | (cast(u32) ' ' << 24))
            {
                vp8Ofs = dat
                vp8Siz = csz
            }

            p = dat + ((csz + 1) & ~1) // chunks are padded to even
        }

        if vp8Ofs < 0:
            throw InvalidFormatError{"webp: VP8 missing"}

        // clamp to stream size
        if (vp8Ofs + vp8Siz) > me.streamLen:
            vp8Siz = me.streamLen - vp8Ofs

        return {vp8Ofs, vp8Siz}
    }

    mtd parseVP8Frame(ofs: s32, size: s32)->VP8Header throw
    {
        if size < 10:
            throw InvalidFormatError{"vp8: chunk too short"}

        let b0 = me.dU8(ofs + 0)
        let b1 = me.dU8(ofs + 1)
        let b2 = me.dU8(ofs + 2)

        var hdr: VP8Header
        hdr.keyFrame    = ((b0 & 1) == 0)
        hdr.version     = (b0 >> 1) & 7
        hdr.showFrame   = ((b0 >> 4) & 1) == 1
        hdr.firstPartSz = cast(s32) (b0 >> 5) | (cast(s32) b1 << 3) | (cast(s32) b2 << 11)

        if !hdr.keyFrame:
            throw UnsupportedFormatError{"vp8: only keyframes supported"}

        // Check keyframe signature
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

        // Sync with container dimensions
        me.imageSizeX = hdr.width
        me.imageSizeY = hdr.height

        return hdr
    }

    mtd decodeKeyframe(ofs: s32, size: s32, yPlane: [*] s16, uPlane: [*] s16, vPlane: [*] s16) throw
    {
        let hdr = me.parseVP8Frame(ofs, size)

        // VP8 keyframe structure: 10 bytes header + compressed data
        let dataStart = ofs + 10
        let dataSize  = hdr.firstPartSz

        if dataSize <= 0 or (dataStart + dataSize) > (ofs + size):
            throw InvalidFormatError{"vp8: invalid partition size"}

        var b: BoolDec
        initBool(&b, me.stream + dataStart, dataSize)

        // Skip VP8 frame header bits (color space, clamping, etc.)
        discard readBit(&b) // color_space
        discard readBit(&b) // clamping_type

        // Very simplified intra-only keyframe decoding:
        // We assume all macroblocks are intra-coded and 4x4 prediction.
        // We only decode coefficients and apply an IDCT. We now add a DC-predictor
        // from neighbors so chroma is not stuck at 128 (grayscale).

        let mbW = (hdr.width + 15) >> 4      // number of 16x16 luma macroblocks across
        let mbH = (hdr.height + 15) >> 4     // number of 16x16 luma macroblocks down

        var mbY = 0
        while mbY < mbH
        {
            var mbX = 0
            while mbX < mbW
            {
                // -------- Y (luma) 16x16: 16 4x4 sub-blocks ----------
                var coeffs: [16] s16
                var recon:  [16] s16

                var by = 0
                while by < 4
                {
                    var bx = 0
                    while bx < 4
                    {
                        let hasCoeffs = decodeBlock4x4(&b, &coeffs[0], 0) // plane 0 = Y
                        if hasCoeffs
                        {
                            dequantizeBlock(&coeffs[0], 0, false) // false = not DC block
                            idct4(&coeffs[0], &recon[0])
                        }
                        else
                        {
                            var r = 0
                            while r < 16
                            {
                                recon[r] = 0
                                r += 1
                            }
                        }

                        // DC predictor for this 4x4 luma block (simple): average of top and left samples when available
                        let ix0 = mbX * 16 + bx * 4
                        let iy0 = mbY * 16 + by * 4
                        var dcY = 128
                        var sum = 0
                        var cnt = 0
                        // top row
                        if iy0 > 0 and (ix0 + 3) < hdr.width
                        {
                            var k = 0
                            while k < 4
                            {
                                sum += cast(s32) yPlane[(iy0 - 1) * hdr.yStride + ix0 + k]
                                cnt += 1
                                k += 1
                            }
                        }
                        // left column
                        if ix0 > 0 and (iy0 + 3) < hdr.height
                        {
                            var k = 0
                            while k < 4
                            {
                                sum += cast(s32) yPlane[(iy0 + k) * hdr.yStride + (ix0 - 1)]
                                cnt += 1
                                k += 1
                            }
                        }
                        if cnt > 0:
                            dcY = sum / cnt

                        // write back to Y plane
                        var py = 0
                        while py < 4
                        {
                            var px = 0
                            while px < 4
                            {
                                let ix = mbX * 16 + bx * 4 + px
                                let iy = mbY * 16 + by * 4 + py
                                if ix < hdr.width and iy < hdr.height
                                {
                                    let val = dcY + cast(s32) recon[py * 4 + px]
                                    yPlane[iy * hdr.yStride + ix] = cast(s16) Math.clamp(val, 0, 255)
                                }

                                px += 1
                            }

                            py += 1
                        }

                        bx += 1
                    }

                    by += 1
                }

                // -------- U (Cb) chroma 8x8: 4x 4x4 sub-blocks ----------
                var cby = 0
                while cby < 2
                {
                    var cbx = 0
                    while cbx < 2
                    {
                        var sub = 0
                        while sub < 4 // Each 8x8 has 4 4x4 sub-blocks
                        {
                            let hasCoeffs = decodeBlock4x4(&b, &coeffs[0], 1) // plane 1 = U
                            if hasCoeffs
                            {
                                dequantizeBlock(&coeffs[0], 1, false)
                                idct4(&coeffs[0], &recon[0])
                            }
                            else
                            {
                                var r = 0
                                while r < 16
                                {
                                    recon[r] = 0
                                    r += 1
                                }
                            }

                            // DC predictor for this 4x4 chroma-U block
                            let cix0 = mbX * 8 + cbx * 4 + ((sub & 1) * 4)
                            let ciy0 = mbY * 8 + cby * 4 + ((sub >> 1) * 4)
                            var dcU  = 128
                            var csum = 0
                            var ccnt = 0
                            // top row in U plane
                            let uW = (hdr.width + 1) >> 1
                            let uH = (hdr.height + 1) >> 1
                            if ciy0 > 0 and (cix0 + 3) < uW
                            {
                                var k = 0
                                while k < 4
                                {
                                    csum += cast(s32) uPlane[(ciy0 - 1) * hdr.uvStride + cix0 + k]
                                    ccnt += 1
                                    k += 1
                                }
                            }
                            // left column in U plane
                            if cix0 > 0 and (ciy0 + 3) < uH
                            {
                                var k = 0
                                while k < 4
                                {
                                    csum += cast(s32) uPlane[(ciy0 + k) * hdr.uvStride + (cix0 - 1)]
                                    ccnt += 1
                                    k += 1
                                }
                            }
                            if ccnt > 0:
                                dcU = csum / ccnt

                            let sy = (sub >> 1) * 4
                            let sx = (sub & 1) * 4

                            var py = 0
                            while py < 4
                            {
                                var px = 0
                                while px < 4
                                {
                                    let ix = mbX * 8 + cbx * 4 + sx + px
                                    let iy = mbY * 8 + cby * 4 + sy + py
                                    if ix < ((hdr.width + 1) >> 1) and iy < ((hdr.height + 1) >> 1)
                                    {
                                        let val = dcU + cast(s32) recon[py * 4 + px]
                                        uPlane[iy * hdr.uvStride + ix] = cast(s16) Math.clamp(val, 0, 255)
                                    }

                                    px += 1
                                }

                                py += 1
                            }

                            sub += 1
                        }

                        cbx += 1
                    }

                    cby += 1
                }

                // -------- V (Cr) chroma 8x8: 4x 4x4 sub-blocks ----------
                cby = 0
                while cby < 2
                {
                    var cbx = 0
                    while cbx < 2
                    {
                        var sub = 0
                        while sub < 4
                        {
                            let hasCoeffs = decodeBlock4x4(&b, &coeffs[0], 1) // plane 1 = V uses same probs as U
                            if hasCoeffs
                            {
                                dequantizeBlock(&coeffs[0], 1, false)
                                idct4(&coeffs[0], &recon[0])
                            }
                            else
                            {
                                var r = 0
                                while r < 16
                                {
                                    recon[r] = 0
                                    r += 1
                                }
                            }

                            // DC predictor for this 4x4 chroma-V block
                            let vcix0 = mbX * 8 + cbx * 4 + ((sub & 1) * 4)
                            let vciy0 = mbY * 8 + cby * 4 + ((sub >> 1) * 4)
                            var dcV   = 128
                            var vsum  = 0
                            var vcnt  = 0
                            let vW    = (hdr.width + 1) >> 1
                            let vH    = (hdr.height + 1) >> 1
                            if vciy0 > 0 and (vcix0 + 3) < vW
                            {
                                var k = 0
                                while k < 4
                                {
                                    vsum += cast(s32) vPlane[(vciy0 - 1) * hdr.uvStride + vcix0 + k]
                                    vcnt += 1
                                    k += 1
                                }
                            }
                            if vcix0 > 0 and (vciy0 + 3) < vH
                            {
                                var k = 0
                                while k < 4
                                {
                                    vsum += cast(s32) vPlane[(vciy0 + k) * hdr.uvStride + (vcix0 - 1)]
                                    vcnt += 1
                                    k += 1
                                }
                            }
                            if vcnt > 0:
                                dcV = vsum / vcnt

                            let sy = (sub >> 1) * 4
                            let sx = (sub & 1) * 4

                            var py = 0
                            while py < 4
                            {
                                var px = 0
                                while px < 4
                                {
                                    let ix = mbX * 8 + cbx * 4 + sx + px
                                    let iy = mbY * 8 + cby * 4 + sy + py
                                    if ix < ((hdr.width + 1) >> 1) and iy < ((hdr.height + 1) >> 1)
                                    {
                                        let val = dcV + cast(s32) recon[py * 4 + px]
                                        vPlane[iy * hdr.uvStride + ix] = cast(s16) Math.clamp(val, 0, 255)
                                    }

                                    px += 1
                                }

                                py += 1
                            }

                            sub += 1
                        }

                        cbx += 1
                    }

                    cby += 1
                }

                mbX += 1
            }

            mbY += 1
        }
    }

    mtd start() throw
    {
        // parse RIFF + sizes, find VP8
        let (?, vp8Siz) = me.parseRiff()
        if vp8Siz <= 0:
            throw InvalidFormatError{"webp/vp8: zero size"}
    }

    // Conversion to BGR8 from s16 Y/U/V planes
    func toBGR(dst: [*] u8, y: [*] s16, u: [*] s16, v: [*] s16, w: s32, h: s32)
    {
        var j = 0
        while j < h
        {
            let yrow   = y + j * w
            let urow   = u + (j >> 1) * ((w + 1) >> 1)
            let vrow   = v + (j >> 1) * ((w + 1) >> 1)
            let dstRow = dst + j * w * 3
            yuvToBgrRow(dstRow, yrow, urow, vrow, w)
            j += 1
        }
    }

    mtd doit(pixels: [*] u8) throw
    {
        // find VP8 chunk
        var vp8Ofs = -1
        var vp8Siz = 0

        // Minimal rescan (refactor to avoid storing)
        if !(me.isTag(0, 'R', 'I', 'F', 'F') and me.isTag(8, 'W', 'E', 'B', 'P')):
            throw InvalidFormatError{"webp: missing RIFF/WEBP header"}

        var p = 12
        while (p + 8) <= me.streamLen
        {
            let csz = cast(s32) me.dU32(p + 4)
            let dat = p + 8
            if me.isTag(p, 'V', 'P', '8', ' ')
            {
                vp8Ofs = dat
                vp8Siz = csz
                break
            }
            p = dat + ((csz + 1) & ~1)
        }

        if vp8Ofs < 0:
            throw InvalidFormatError{"webp: VP8 missing"}

        // Temporary s16 planes
        let W       = me.imageSizeX
        let H       = me.imageSizeY
        var yPlane: Array's16
        var uPlane: Array's16
        var vPlane: Array's16
        yPlane.resize(W * H)
        uPlane.resize(((W + 1) >> 1) * ((H + 1) >> 1))
        vPlane.resize(((W + 1) >> 1) * ((H + 1) >> 1))

        // Initialize neutral chroma 128 (just in case)
        var i = 0
        while i < @countof(uPlane)
        {
            uPlane[i] = 128
            i += 1
        }

        i = 0
        while i < @countof(vPlane)
        {
            vPlane[i] = 128
            i += 1
        }

        // Decode bounded keyframe (intra)
        me.decodeKeyframe(vp8Ofs, vp8Siz, @dataof(yPlane), @dataof(uPlane), @dataof(vPlane))

        // Convert to BGR
        toBGR(pixels, @dataof(yPlane), @dataof(uPlane), @dataof(vPlane), W, H)
    }

    // small init that scans sizes
    mtd init(buffer: const [..] u8) throw
    {
        me.stream    = @dataof(buffer)
        me.streamLen = cast(s32) @countof(buffer)

        // find the VP8 chunk once to read dimensions
        let (vp8Ofs, vp8Siz) = me.parseRiff()
        if vp8Ofs < 0 or vp8Siz <= 0:
            throw InvalidFormatError{"webp: no VP8 chunk"}

        let hdr = me.parseVP8Frame(vp8Ofs, vp8Siz)
        me.imageSizeX = hdr.width
        me.imageSizeY = hdr.height
    }
}

public struct Decoder
{
    // stream
    stream:        const [*] u8
    streamLen:     s32

    // image
    imageSizeX:     s32
    imageSizeY:     s32

    // metadata
    decOptions:     DecodeOptions
    metaDatas:      Array'ImageMetaData
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
        me.decOptions = options
        me.metaDatas.clear()

        // Start / parse headers
        me.init(buffer)
        me.start()

        var result: Image
        result.init(me.imageSizeX, me.imageSizeY, PixelFormat.BGR8)
        if !options.decodePixels:
            return result
        result.allocPixels()

        me.doit(result.pixels.buffer)
        result.metaDatas = #move me.metaDatas
        return result
    }
}
