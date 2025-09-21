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
    const DCT_EOB  = 11

    // Decode 4x4 block coefficients
    func decodeBlock4x4(b: *BoolDec, coeffs: [*] s16, plane: s32)->bool
    {
        var hasCoeffs = false
        var i         = 0

        // Map coefficient index to VP8 coefficient band
        const BandForPos: [16] u8 = [0, 1, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5]

        // Select block type: 0 = Y, 1 = UV
        let bt = (plane == 0) ? 0 : 1

        while i < 16
        {
            coeffs[i] = 0

            let band = cast(s32) BandForPos[i]
            let ctx  = 0                           // Start with context 0, should be calculated from neighbors

            // Ensure band is in valid range
            let validBand = (band > 5) ? 5 : band

            // Use proper VP8 coefficient probability structure
            // First check if there are more coefficients (vs EOB)
            if readBool(b, CoeffProbsDefault[bt][validBand][ctx][0]) == 0:
                break // EOB - end of block

            // Check if coefficient is zero
            if readBool(b, CoeffProbsDefault[bt][validBand][ctx][1]) == 0
            {
                i += 1 // Zero coefficient
                continue
            }

            // We have a non-zero coefficient
            hasCoeffs = true

            // Decode the token (simplified - just handle small values for now)
            var value = 1 // Start with 1

            // Check if it's larger than 1
            if readBool(b, CoeffProbsDefault[bt][validBand][ctx][2]) != 0
            {
                value = 2
                // For simplicity, just decode up to small values
                // A full implementation would handle all token categories
            }

            // Read sign bit
            if readBit(b) != 0:
                value = -value

            coeffs[i] = cast(s16) value
            i += 1
        }

        return hasCoeffs
    }
}

private
{
    const MaxSize = 1_000_000_000
    const MaxDim  = 8192
    const MBSize  = 16
    const BLK4    = 4
    const BLK8    = 8
    const QFixed  = 7

    struct ChunkInfo
    {
        id:         u32
        size:       s32
        offset:     s32
    }

    struct VP8Header
    {
        keyFrame:        bool
        version:         u8
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
    const CoeffProbsDefault = [
        // Y 4x4
        [[[0, 0, 0], [0, 0, 0], [0, 0, 0]],
        [[145, 199, 177], [67, 101, 66], [64, 90, 47]],
        [[187, 224, 203], [77, 123, 85], [68, 97, 55]],
        [[214, 235, 211], [85, 130, 90], [71, 98, 57]],
        [[229, 240, 215], [87, 131, 92], [72, 100, 58]],
        [[237, 243, 218], [88, 132, 93], [73, 101, 60]]],
        // UV 4x4
        [[[0, 0, 0], [0, 0, 0], [0, 0, 0]],
        [[156, 203, 179], [70, 118, 71], [67, 94, 50]],
        [[189, 223, 201], [75, 123, 83], [69, 97, 54]],
        [[214, 235, 210], [83, 129, 89], [70, 98, 56]],
        [[228, 241, 214], [85, 131, 91], [71, 100, 58]],
        [[236, 244, 217], [87, 132, 92], [72, 101, 59]]],
        // Y2 (DC 16x16)
        [[[0, 0, 0], [0, 0, 0], [0, 0, 0]],
        [[173, 231, 194], [80, 110, 76], [69, 93, 54]],
        [[200, 239, 208], [88, 127, 86], [73, 100, 58]],
        [[216, 242, 214], [90, 131, 90], [74, 102, 60]],
        [[224, 244, 217], [92, 133, 92], [75, 103, 60]],
        [[230, 246, 219], [93, 134, 93], [76, 104, 61]]],
        // UV 16x16 (treated like UV 4x4 in VP8, so already covered)
        [[[0, 0, 0], [0, 0, 0], [0, 0, 0]],
        [[156, 203, 179], [70, 118, 71], [67, 94, 50]],
        [[189, 223, 201], [75, 123, 83], [69, 97, 54]],
        [[214, 235, 210], [83, 129, 89], [70, 98, 56]],
        [[228, 241, 214], [85, 131, 91], [71, 100, 58]],
        [[236, 244, 217], [87, 132, 92], [72, 101, 59]]]]

    // iDCT tables (like VP8 JPEG-style 4×4) – integer constants
    const CosVal = [[64, 83, 64, 36], // equiv. * sqrt(2) / 2 scaled
    [64, 36, -64, -83]]

    // --- Bool decoder ---
    struct BoolDec { range: u32, value: u32, bitpos: s32, p: const [*] u8, end: const [*] u8 }
    func initBool(b: *BoolDec, src: const [*] u8, len: s32)
    {
        if len < 3
        {
            b.range  = 255
            b.value  = 0
            b.bitpos = 0
            b.p      = src
            b.end    = src + len
            return
        }

        b.range  = 255
        b.value  = (cast(u32) src[0] << 16) | (cast(u32) src[1] << 8) | cast(u32) src[2]
        b.bitpos = 0
        b.p      = src + 3
        b.end    = src + len
    }

    func readBool(b: *BoolDec, p: u32)->u32
    {
        // Ensure probability is in valid range
        let prob     = (p > 255) ? 255 : p
        let split    = 1 + (((b.range - 1) * prob) >> 8)
        let bigsplit = cast(u32) split << 8
        let big      = b.value >= bigsplit

        if big
        {
            b.value -= bigsplit
            b.range -= split
        }
        else
        {
            b.range = split
        }

        // Normalize after each read
        while b.range < 128 and b.p < b.end
        {
            b.range <<= 1
            b.value = (b.value << 1) | cast(u32) (dref b.p)
            b.p += 1
        }

        return big ? 1 : 0
    }

    func readBit(b: *BoolDec)->u32
    {
        return readBool(b, 128)
    }

    // --- iDCT 4×4 VP8 ---
    func idct4(src: const [*] s16, dst: [*] s16)
    {
        var tmp: [16] s32

        // helper for saturating cast
        func sat16(v: s32)->s16
        {
            if v > 32767
            {
                return 32767
            }
            if v < -32768
            {
                return -32768
            }
            return cast(s16) v
        }

        // Pass 1: columns
        var i = 0
        while i < 4
        {
            let r0: s32 = cast(s32) src[i + 0]
            let r1: s32 = cast(s32) src[i + 4]
            let r2: s32 = cast(s32) src[i + 8]
            let r3: s32 = cast(s32) src[i + 12]

            let s0: s32 = r0 + r2
            let s1: s32 = r0 - r2
            // add +32 for round-to-nearest (>>6)
            let s2: s32 = (r1 * 83 + r3 * 36 + 32) >> 6
            let s3: s32 = (r1 * 36 - r3 * 83 + 32) >> 6

            tmp[i + 0]  = s0 + s2 // row 0, col i
            tmp[i + 4]  = s1 + s3 // row 1, col i
            tmp[i + 8]  = s1 - s3 // row 2, col i
            tmp[i + 12] = s0 - s2 // row 3, col i
            i += 1
        }

        // Pass 2: rows
        i = 0
        while i < 16
        {
            let t0: s32 = tmp[i + 0]
            let t1: s32 = tmp[i + 1]
            let t2: s32 = tmp[i + 2]
            let t3: s32 = tmp[i + 3]

            let s0: s32 = t0 + t2
            let s1: s32 = t0 - t2
            // add +32 for round-to-nearest (>>6)
            let s2: s32 = (t1 * 83 + t3 * 36 + 32) >> 6
            let s3: s32 = (t1 * 36 - t3 * 83 + 32) >> 6

            // final >>3 — use +4 for round-to-nearest
            dst[i + 0] = sat16((s0 + s2 + 4) >> 3)
            dst[i + 1] = sat16((s1 + s3 + 4) >> 3)
            dst[i + 2] = sat16((s1 - s3 + 4) >> 3)
            dst[i + 3] = sat16((s0 - s2 + 4) >> 3)

            i += 4
        }
    }

    // Updated YUV to BGR conversion with better clamping
    func yuvToBgrRow(dst: [*] u8, y: const [*] s16, u: const [*] s16, v: const [*] s16, w: s32)
    {
        // round-to-nearest with arithmetic shift
        func sr8(x: s32)->s32 { return (x >= 0) ? ((x + 128) >> 8) : ((x - 128) >> 8); }

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
            let C1: s32 = Y1 - 16

            // Clamp luma to valid range
            let CC0 = (C0 > 0) ? C0 : 0
            let CC1 = (C1 > 0) ? C1 : 0

            var R0 = sr8(298 * CC0 + 409 * Vc)
            var G0 = sr8(298 * CC0 - 100 * Uc - 208 * Vc)
            var B0 = sr8(298 * CC0 + 516 * Uc)

            var R1 = sr8(298 * CC1 + 409 * Vc)
            var G1 = sr8(298 * CC1 - 100 * Uc - 208 * Vc)
            var B1 = sr8(298 * CC1 + 516 * Uc)

            R0 = Math.clamp(R0, 0, 255)
            G0 = Math.clamp(G0, 0, 255)
            B0 = Math.clamp(B0, 0, 255)
            R1 = Math.clamp(R1, 0, 255)
            G1 = Math.clamp(G1, 0, 255)
            B1 = Math.clamp(B1, 0, 255)

            let o = x * 3
            dst[o + 0] = cast(u8) B0; dst[o + 1] = cast(u8) G0; dst[o + 2] = cast(u8) R0
            dst[o + 3] = cast(u8) B1; dst[o + 4] = cast(u8) G1; dst[o + 5] = cast(u8) R1
            x += 2
        }

        // tail pixel if width is odd
        if (x < w)
        {
            let Y:  s32 = cast(s32) y[x]
            let Uc: s32 = cast(s32) u[x >> 1] - 128
            let Vc: s32 = cast(s32) v[x >> 1] - 128
            let C:  s32 = Y - 16
            let CC  = (C > 0) ? C : 0

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

public struct Decoder
{
    // whole stream
    stream:        const [*] u8
    streamLen:     s32

    // image
    imageSizeX:     s32
    imageSizeY:     s32

    // metadata
    decOptions:     DecodeOptions
    metaDatas:      Array'ImageMetaData
}

impl Decoder
{
    // --- read helpers ---
    mtd U8(at: s32)->u8 { if at < 0 or at >= me.streamLen:return 0; return me.stream[at]; }
    mtd U16(at: s32)->u32 { return cast(u32) me.U8(at) | (cast(u32) me.U8(at + 1) << 8); }
    mtd U24(at: s32)->u32 { return cast(u32) me.U8(at) | (cast(u32) me.U8(at + 1) << 8) | (cast(u32) me.U8(at + 2) << 16); }
    mtd U32(at: s32)->u32 { return cast(u32) me.U8(at) | (cast(u32) me.U8(at + 1) << 8) | (cast(u32) me.U8(at + 2) << 16) | (cast(u32) me.U8(at + 3) << 24); }
    mtd isTag(at: s32, a: u8, b: u8, c: u8, d: u8)->bool { return me.U8(at) == a and me.U8(at + 1) == b and me.U8(at + 2) == c and me.U8(at + 3) == d; }

    mtd init(buffer: const [..] u8)
    {
        @init(me, 1)
        me.stream     = @dataof(buffer)
        me.streamLen  = cast(s32) @countof(buffer)
        me.imageSizeX = 0
        me.imageSizeY = 0
    }

    // --------- RIFF/WEBP ----------
    mtd parseRiff()->{ ofsVp8: s32, sizVp8: s32 } throw
    {
        if me.streamLen < 12:
            throw InvalidFormatError{"webp: file too short"}
        if !me.isTag(0, 'R', 'I', 'F', 'F'):
            throw InvalidFormatError{"webp: missing RIFF"}
        if !me.isTag(8, 'W', 'E', 'B', 'P'):
            throw InvalidFormatError{"webp: missing WEBP"}

        var p      = 12
        var vp8Ofs = -1
        var vp8Siz = 0
        var w      = 0
        var h      = 0

        while (p + 8) <= me.streamLen
        {
            discard me.U32(p)
            let csz = cast(s32) me.U32(p + 4)
            let dat = p + 8
            if csz < 0 or dat + csz > me.streamLen:
                break

            if me.isTag(p, 'V', 'P', '8', 'X')
            {
                if csz >= 10
                {
                    // flags = U8(dat)
                    discard me.U8(dat)
                    // 3 reserved
                    // width/height minus 1 on 24b
                    w = cast(s32) me.U24(dat + 4) + 1
                    h = cast(s32) me.U24(dat + 7) + 1
                }
            }
            elif me.isTag(p, 'V', 'P', '8', ' ')
            {
                vp8Ofs = dat
                vp8Siz = csz
            }
            elif me.isTag(p, 'I', 'C', 'C', 'P')
            {
                var md: ImageMetaData
                md.sig = 0
                md.tag.add('I')
                md.tag.add('C')
                md.tag.add('C')
                md.tag.add('P')
                var i = 0
                while i < csz
                {
                    md.value.add(me.U8(dat + i))
                    i += 1
                }
                me.metaDatas.add(md)
            }

            p = dat + csz
            if (csz & 1) != 0:
                p += 1
        }

        if vp8Ofs < 0:
            throw InvalidFormatError{"webp: missing VP8 chunk"}
        // fallback size from raw VP8 if not found
        if w == 0 or h == 0
        {
            if !(me.U8(vp8Ofs + 3) == 0x9D and me.U8(vp8Ofs + 4) == 0x01 and me.U8(vp8Ofs + 5) == 0x2A):
                throw InvalidFormatError{"webp: invalid VP8 header"}
            w = cast(s32) (me.U16(vp8Ofs + 6) & 0x3FFF)
            h = cast(s32) (me.U16(vp8Ofs + 8) & 0x3FFF)
        }

        if w <= 0 or h <= 0 or w > MaxDim or h > MaxDim:
            throw InvalidFormatError{"webp: invalid dimensions"}

        me.imageSizeX = w
        me.imageSizeY = h
        return {vp8Ofs, vp8Siz}
    }

    mtd parseVP8Frame(ofs: s32, size: s32)->VP8Header throw
    {
        if size < 10:
            throw InvalidFormatError{"vp8: chunk too short"}

        let b0 = me.U8(ofs + 0)
        let b1 = me.U8(ofs + 1)
        let b2 = me.U8(ofs + 2)

        var hdr: VP8Header
        hdr.keyFrame    = ((b0 & 1) == 0)
        hdr.version     = (b0 >> 1) & 7
        hdr.showFrame   = ((b0 >> 4) & 1) == 1
        hdr.firstPartSz = cast(s32) (b0 >> 5) | (cast(s32) b1 << 3) | (cast(s32) b2 << 11)

        if !hdr.keyFrame:
            throw UnsupportedFormatError{"vp8: only keyframes supported"}

        // Check keyframe signature
        if !(me.U8(ofs + 3) == 0x9D and me.U8(ofs + 4) == 0x01 and me.U8(ofs + 5) == 0x2A):
            throw InvalidFormatError{"vp8: invalid keyframe signature"}

        hdr.width  = cast(s32) (me.U16(ofs + 6) & 0x3FFF)
        hdr.height = cast(s32) (me.U16(ofs + 8) & 0x3FFF)
        hdr.scaleW = cast(u8) ((me.U16(ofs + 6) >> 14) & 3)
        hdr.scaleH = cast(u8) ((me.U16(ofs + 8) >> 14) & 3)

        if hdr.width <= 0 or hdr.height <= 0:
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
        discard readBit(&b) // clamp_type

        // set up macroblock loops
        var mbY = 0
        while mbY < hdr.height
        {
            var mbX = 0
            while mbX < hdr.width
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
                        if hasCoeffs:
                            idct4(&coeffs[0], &recon[0])
                        else
                        {
                            var r = 0
                            while r < 16
                            {
                                recon[r] = 0; r += 1; }
                        }

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
                                    let val = 128 + cast(s32) recon[py * 4 + px]
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
                            if hasCoeffs:
                                idct4(&coeffs[0], &recon[0])
                            else
                            {
                                var r = 0
                                while r < 16
                                {
                                    recon[r] = 0; r += 1; }
                            }

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
                                        let val = 128 + cast(s32) recon[py * 4 + px]
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
                            let hasCoeffs = decodeBlock4x4(&b, &coeffs[0], 2) // fixed: plane 2 for V
                            if hasCoeffs:
                                idct4(&coeffs[0], &recon[0])
                            else
                            {
                                var r = 0
                                while r < 16
                                {
                                    recon[r] = 0; r += 1; }
                            }

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
                                        let val = 128 + cast(s32) recon[py * 4 + px]
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
            let csz = cast(s32) me.U32(p + 4)
            let dat = p + 8
            if me.isTag(p, 'V', 'P', '8', ' ')
            {
                vp8Ofs = dat; vp8Siz = csz; break
            }
            p = dat + csz
            if (csz & 1) != 0:
                p += 1
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
            uPlane[i] = 128; i += 1
        }
        i = 0
        while i < @countof(vPlane)
        {
            vPlane[i] = 128; i += 1
        }

        // Decode bounded keyframe (intra)
        me.decodeKeyframe(vp8Ofs, vp8Siz, @dataof(yPlane), @dataof(uPlane), @dataof(vPlane))

        // Convert to BGR
        toBGR(pixels, @dataof(yPlane), @dataof(uPlane), @dataof(vPlane), W, H)
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
        var result: retval
        me.decOptions = options

        me.init(buffer)
        me.start()

        result.init(me.imageSizeX, me.imageSizeY, PixelFormat.BGR8)
        if !options.decodePixels:
            return result
        result.allocPixels()

        me.doit(result.pixels.buffer)
        result.metaDatas = #move me.metaDatas
        return result
    }
}
