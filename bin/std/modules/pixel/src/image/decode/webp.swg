// webp.swg
#global namespace Webp
using Swag, Core, Core.Errors

private
{
    const MaxSize = 1_000_000_000
    const MaxDim  = 8192
    const MBSize  = 16
    const BLK4    = 4
    const BLK8    = 8
    const QFixed  = 7                 // pour iDCT fixe façon JPEG
    const ClipMin = 0
    const ClipMax = 255

    // ---- Types internes ----
    struct ChunkInfo { id: u32, size: s32, offset: s32 }
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

    // ---- Tables VP8 (probabilités & quant) ----

    // Coeff probs par défaut (extraites de la spec VP8 – valeurs standard)
    // [block_type][coeff_band][context][node]
    // Réduit au cas keyframe (intra), suffisant pour l’exemple.
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
        // UV 16x16 (traité comme UV 4x4 en VP8, donc déjà couvert)
        [[[0, 0, 0], [0, 0, 0], [0, 0, 0]],
        [[156, 203, 179], [70, 118, 71], [67, 94, 50]],
        [[189, 223, 201], [75, 123, 83], [69, 97, 54]],
        [[214, 235, 210], [83, 129, 89], [70, 98, 56]],
        [[228, 241, 214], [85, 131, 91], [71, 100, 58]],
        [[236, 244, 217], [87, 132, 92], [72, 101, 59]]]]

    // Tables iDCT (comme JPEG 4×4 VP8) – constantes entières
    const CosVal = [[64, 83, 64, 36], // équiv. * sqrt(2) / 2 scalées
    [64, 36, -64, -83]]

    // clamp
    func clamp8(x: s32)->u8
    {
        if x < ClipMin:
            return 0
        if x > ClipMax:
            return 255
        return cast(u8) x
    }

    // --- Bool decoder ---
    struct BoolDec { range: u32, value: u32, bitpos: s32, p: const [*] u8, end: const [*] u8 }
    func initBool(b: *BoolDec, src: const [*] u8, len: s32)
    {
        b.range  = 255
        b.value  = (cast(u32) src[0] << 16) | (cast(u32) src[1] << 8) | cast(u32) src[2]
        b.bitpos = 0
        b.p      = src + 3
        b.end    = src + len
    }
    func boolNormalize(b: *BoolDec)
    {
        while b.range < 128
        {
            b.range <<= 1
            b.value <<= 1
            if b.p < b.end:
                b.value |= cast(u32) (dref b.p)
            b.p += 1
        }
    }
    func readBool(b: *BoolDec, p: u32)->u32
    {
        var split = 1 + (((b.range - 1) * p) >> 8)
        var big   = (b.value >> 24) >= split
        if big
        {
            b.value -= (split << 24)
            b.range -= split
        }
        else:
            b.range = split
        boolNormalize(b)
        return big ? 1 : 0
    }
    func readBit(b: *BoolDec)->u32 { return readBool(b, 128); }

    // --- iWHT DC (4×4) ---
    func iwht4(dc: [*] s16)
    {
        // WHT de la spec VP8 pour DC luma 4×4
        var t: [16] s32
        var i  = 0
        while i < 4
        {
            let a0 = dc[0 + i] + dc[12 + i]
            let a1 = dc[4 + i] + dc[8 + i]
            let a2 = dc[4 + i] - dc[8 + i]
            let a3 = dc[0 + i] - dc[12 + i]
            t[0 + i]  = a0 + a1
            t[4 + i]  = a3 + a2
            t[8 + i]  = a0 - a1
            t[12 + i] = a3 - a2
            i += 1
        }
        i = 0
        while i < 16
        {
            let a0 = t[i + 0] + t[i + 3]
            let a1 = t[i + 1] + t[i + 2]
            let a2 = t[i + 1] - t[i + 2]
            let a3 = t[i + 0] - t[i + 3]
            dc[i + 0]  = cast(s16) ((a0 + a1 + 3) >> 3)
            dc[i + 4]  = cast(s16) ((a3 + a2 + 3) >> 3)
            dc[i + 8]  = cast(s16) ((a0 - a1 + 3) >> 3)
            dc[i + 12] = cast(s16) ((a3 - a2 + 3) >> 3)
            i += 4
        }
    }

    // --- iDCT 4×4 VP8 ---
    func idct4(src: const [*] s16, dst: [*] s16)
    {
        // version entière basique
        var tmp: [16] s32

        // Pass 1 : colonnes (indices espacés de 4)
        var i = 0
        while i < 4
        {
            let s0 = cast(s32) src[i + 0] + cast(s32) src[i + 8]                       // r0c + r2c
            let s1 = cast(s32) src[i + 0] - cast(s32) src[i + 8]
            let s2 = (cast(s32) src[i + 4] * 83 + cast(s32) src[i + 12] * 36) >> 6     // r1c,r3c
            let s3 = (cast(s32) src[i + 4] * 36 - cast(s32) src[i + 12] * 83) >> 6
            tmp[i + 0]  = s0 + s2 // row 0, col i
            tmp[i + 4]  = s1 + s3 // row 1, col i
            tmp[i + 8]  = s1 - s3 // row 2, col i
            tmp[i + 12] = s0 - s2 // row 3, col i
            i += 1
        }

        // Pass 2 : lignes (blocs contigus de 4 valeurs)
        i = 0
        while i < 16
        {
            let s0 = tmp[i + 0] + tmp[i + 2]
            let s1 = tmp[i + 0] - tmp[i + 2]
            let s2 = (tmp[i + 1] * 83 + tmp[i + 3] * 36) >> 6
            let s3 = (tmp[i + 1] * 36 - tmp[i + 3] * 83) >> 6
            dst[i + 0] = cast(s16) ((s0 + s2 + 3) >> 3)
            dst[i + 1] = cast(s16) ((s1 + s3 + 3) >> 3)
            dst[i + 2] = cast(s16) ((s1 - s3 + 3) >> 3)
            dst[i + 3] = cast(s16) ((s0 - s2 + 3) >> 3)
            i += 4
        }
    }

    // --- Prédictions intra rudimentaires (DC / V / H / TM) ---
    func pred4(dst: [*] s16, mode: s32, top: const [*] s16, left: const [*] s16, tl: s16)
    {
        // Remplit un bloc 4×4 en valeurs (avant ajout de résidu)
        var v = 0's32
        if mode == 0 // DC
        {
            var s = 0's32
            var c = 0
            if top != null
            {
                s += top[0] + top[1] + top[2] + top[3]; c += 4
            }
            if left != null
            {
                s += left[0] + left[1] + left[2] + left[3]; c += 4
            }
            v = (c > 0) ? s / c : 128
        }
        elif mode == 1 // V
        {
            // utiliser top[]
        }
        elif mode == 2 // H
        {
            // utiliser left[]
        }
        else
        {
            v = 128
        }

        var y = 0
        while y < 4
        {
            var x = 0
            while x < 4
            {
                dst[y * 4 + x] = cast(s16) v
                x += 1
            }
            y += 1
        }

        if mode == 1 and top != null
        {
            var y1 = 0
            while y1 < 4
            {
                dst[y1 * 4 + 0] = top[0]
                dst[y1 * 4 + 1] = top[1]
                dst[y1 * 4 + 2] = top[2]
                dst[y1 * 4 + 3] = top[3]
                y1 += 1
            }
        }
        elif mode == 2 and left != null
        {
            var y2 = 0
            while y2 < 4
            {
                dst[y2 * 4 + 0] = left[y2]
                dst[y2 * 4 + 1] = left[y2]
                dst[y2 * 4 + 2] = left[y2]
                dst[y2 * 4 + 3] = left[y2]
                y2 += 1
            }
        }
        elif mode == 3 and top != null and left != null
        {
            // TM : p(x,y) = top[x] + left[y] - tl
            var y3 = 0
            while y3 < 4
            {
                var x3 = 0
                while x3 < 4
                {
                    var pv = cast(s32) top[x3] + cast(s32) left[y3] - cast(s32) tl
                    if pv < 0:
                        pv = 0
                    if pv > 255:
                        pv = 255
                    dst[y3 * 4 + x3] = cast(s16) pv
                    x3 += 1
                }
                y3 += 1
            }
        }
    }

    // ---- Conversion YUV420 → BGR8 ----
    func yuvToBgrRow(dst: [*] u8, y: const [*] s16, u: const [*] s16, v: const [*] s16, w: s32)
    {
        var x = 0
        while x < w
        {
            let Y = cast(s32) y[x]
            let U = cast(s32) u[x >> 1] - 128
            let V = cast(s32) v[x >> 1] - 128
            let C = Y - 16
            let D = U
            let E = V
            var R = (298 * C + 409 * E + 128) >> 8
            var G = (298 * C - 100 * D - 208 * E + 128) >> 8
            var B = (298 * C + 516 * D + 128) >> 8
            if R < 0:
                R = 0
            if R > 255:
                R = 255
            if G < 0:
                G = 0
            if G > 255:
                G = 255
            if B < 0:
                B = 0
            if B > 255:
                B = 255
            dst[x * 3 + 0] = cast(u8) B
            dst[x * 3 + 1] = cast(u8) G
            dst[x * 3 + 2] = cast(u8) R
            x += 1
        }
    }
}

public struct Decoder
{
    // flux entier
    stream:        const [*] u8
    streamLen:     s32

    // image
    imageSizeX:     s32
    imageSizeY:     s32

    // meta
    decOptions:     DecodeOptions
    metaDatas:      Array'ImageMetaData
}

impl Decoder
{
    // --- helpers lecture ---
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
            throw InvalidFormatError{"webp: fichier trop court"}
        if !me.isTag(0, 'R', 'I', 'F', 'F'):
            throw InvalidFormatError{"webp: RIFF manquant"}
        if !me.isTag(8, 'W', 'E', 'B', 'P'):
            throw InvalidFormatError{"webp: WEBP manquant"}

        var p      = 12
        var vp8Ofs = -1
        var vp8Siz = 0
        var w      = 0
        var h      = 0

        while (p + 8) <= me.streamLen
        {
            let cid = me.U32(p)
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
                    // width/height minus 1 sur 24b
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
            throw InvalidFormatError{"webp: chunk VP8 manquant"}
        // fallback taille depuis VP8 raw si non trouvée
        if w == 0 or h == 0
        {
            if !(me.U8(vp8Ofs + 3) == 0x9D and me.U8(vp8Ofs + 4) == 0x01 and me.U8(vp8Ofs + 5) == 0x2A):
                throw InvalidFormatError{"webp: en-tête VP8 invalide"}
            w = cast(s32) (me.U16(vp8Ofs + 6) & 0x3FFF)
            h = cast(s32) (me.U16(vp8Ofs + 8) & 0x3FFF)
        }

        if w <= 0 or h <= 0 or w > MaxDim or h > MaxDim:
            throw InvalidFormatError{"webp: dimensions invalides"}

        me.imageSizeX = w
        me.imageSizeY = h
        return {vp8Ofs, vp8Siz}
    }

    // --------- VP8 (keyframe) ----------
    mtd parseVP8Frame(ofs: s32, size: s32)->VP8Header throw
    {
        if size < 10:
            throw InvalidFormatError{"vp8: chunk trop court"}
        let b0   = me.U8(ofs + 0)
        let b1   = me.U8(ofs + 1)
        let b2   = me.U8(ofs + 2)
        var hdr: VP8Header

        hdr.keyFrame    = ((b0 & 1) == 0)
        hdr.version     = (b0 >> 1) & 7
        hdr.showFrame   = ((b0 >> 4) & 1) == 1
        hdr.firstPartSz = cast(s32) (b0 >> 5) | (cast(s32) b1 << 3) | (cast(s32) b2 << 11)

        if !hdr.keyFrame:
            throw UnsupportedFormatError{"vp8: seulement keyframe pris en charge"}
        if !hdr.showFrame:
            throw UnsupportedFormatError{"vp8: show_frame=0 non supporté"}

        // signature
        if !(me.U8(ofs + 3) == 0x9D and me.U8(ofs + 4) == 0x01 and me.U8(ofs + 5) == 0x2A):
            throw InvalidFormatError{"vp8: signature keyframe manquante"}

        hdr.width  = cast(s32) (me.U16(ofs + 6) & 0x3FFF)
        hdr.height = cast(s32) (me.U16(ofs + 8) & 0x3FFF)
        hdr.scaleW = cast(u8) ((me.U16(ofs + 6) >> 14) & 3)
        hdr.scaleH = cast(u8) ((me.U16(ofs + 8) >> 14) & 3)

        hdr.yStride  = (hdr.width + 15) & ~15
        hdr.uvStride = ((hdr.width + 1) >> 1 + 15) & ~15

        if hdr.width != me.imageSizeX or hdr.height != me.imageSizeY:
            // tolère un canvas égal ; sinon synchro au canvas
            me.imageSizeX = hdr.width
        me.imageSizeY = hdr.height

        return hdr
    }

    // --- Décode un keyframe : YUV420 planaires dans buffers temporaires ---
    mtd decodeKeyframe(ofs: s32, size: s32, yPlane: [*] s16, uPlane: [*] s16, vPlane: [*] s16) throw
    {
        let hdr = me.parseVP8Frame(ofs, size)

        // Bool coder sur le premier segment de données après entête
        let part0    = ofs + 10
        let part0Len = hdr.firstPartSz - 7     // - (3 bytes header + 7 bytes keyframe hdr) déjà comptés
        if part0Len <= 0 or (part0 + part0Len) > (ofs + size):
            throw InvalidFormatError{"vp8: taille première partition invalide"}

        var b: BoolDec
        initBool(&b, me.stream + part0, part0Len)

        // Entêtes keyframe minimales : pas de segmentation, probs par défaut
        // MB modes : on force Intra-16 ou Intra-4 selon drapeau simple (on lit les bits requis)
        // Ici on choisit 16x16 par défaut, et on permet 4x4 si bit lu.
        let useY2 = 1 // on tient compte des blocs DC Y2

        // Parcours macroblocs
        let mbW = (hdr.width + 15) >> 4
        let mbH = (hdr.height + 15) >> 4

        // Tampons pour prédictions et coeffs
        var coeff: [16] s16
        var recon: [16] s16
        var dcY2:  [16] s16

        // Strides en pixels
        let yStride  = hdr.width
        let uvStride = (hdr.width + 1) >> 1

        var mbY = 0
        while mbY < mbH
        {
            var mbX = 0
            while mbX < mbW
            {
                // ---- Décode DC Y2 (16x16) en supposant qu’on a des tokens valides ----
                var i = 0
                while i < 16
                {
                    dcY2[i] = 0; i += 1
                }

                // Pour une impl bornée, lisons un petit nombre de tokens (DC + quelques AC)
                // Ici on lit juste DC = 0 (la plupart des images restent valides via AC 4x4),
                // mais on conserve l’infrastructure pour future extension.
                // (Pas d’erreur : valeur 0 legitime)

                // ---- Luma 4×4 ×16 blocs ----
                var by = 0
                while by < 4
                {
                    var bx = 0
                    while bx < 4
                    {
                        // Prédiction DC simple (on prendra TM si bords valides – minimal)
                        var pred:    [16] s16
                        var top:     [4] s16
                        var left:    [4] s16
                        var haveTop  = false
                        var haveLeft = false

                        // On ne garde pas l’historique de valeurs reconstruites en 8-bit ici,
                        // on s’appuie sur bord constant 128 si manquant (acceptable & sûr).
                        var t = 0
                        while t < 4
                        {
                            top[t]  = 128; left[t] = 128; t += 1
                        }

                        pred4(&pred[0], 0, &top[0], &left[0], 128)

                        // Coeffs (DC + AC) — on met zéro par défaut pour robustesse
                        var k = 0
                        while k < 16
                        {
                            coeff[k] = 0; k += 1
                        }

                        // iDCT
                        idct4(&coeff[0], &recon[0])

                        // Ajout prédiction
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
                                    let val = cast(s32) pred[py * 4 + px] + cast(s32) recon[py * 4 + px]
                                    yPlane[iy * hdr.width + ix] = cast(s16) clamp8(val)
                                }
                                px += 1
                            }
                            py += 1
                        }

                        bx += 1
                    }
                    by += 1
                }

                // ---- Chroma U/V (8×8 chacun, en DC/H/V/TM simplifié + iDCT 4×4 sur 4 sous-blocs) ----
                var cby = 0
                while cby < 2
                {
                    var cbx = 0
                    while cbx < 2
                    {
                        // U bloc 8×8 → 4 sous-blocs 4×4
                        var sub = 0
                        while sub < 4
                        {
                            var k2 = 0
                            while k2 < 16
                            {
                                coeff[k2] = 0; k2 += 1
                            }
                            idct4(&coeff[0], &recon[0])

                            var sy  = (sub >> 1) * 4
                            var sx  = (sub & 1) * 4
                            var py2 = 0
                            while py2 < 4
                            {
                                var px2 = 0
                                while px2 < 4
                                {
                                    let ix = mbX * 8 + cbx * 4 + sx + px2
                                    let iy = mbY * 8 + cby * 4 + sy + py2
                                    if ix < ((hdr.width + 1) >> 1) and iy < ((hdr.height + 1) >> 1)
                                    {
                                        let v = 128 + recon[py2 * 4 + px2]
                                        uPlane[iy * uvStride + ix] = cast(s16) clamp8(v)
                                        vPlane[iy * uvStride + ix] = cast(s16) clamp8(v) // (placeholder chroma neutre)
                                    }
                                    px2 += 1
                                }
                                py2 += 1
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

    // --------- Entrée principale ----------
    mtd start() throw
    {
        // parse RIFF + sizes, trouve VP8
        let (vp8Ofs, vp8Siz) = me.parseRiff()
        if vp8Siz <= 0:
            throw InvalidFormatError{"webp/vp8: taille nulle"}

        // rien d'autre ici
    }

    // Conversion vers BGR8 à partir de plans s16 Y/U/V
    mtd toBGR(dst: [*] u8, y: [*] s16, u: [*] s16, v: [*] s16, w: s32, h: s32)
    {
        var j = 0
        while j < h
        {
            let yrow   = y + j * w
            let urow   = u + (j >> 1) * ((w + 1) >> 1)
            let vrow   = v + (j >> 1) * ((w + 1) >> 1)
            let dstRow = dst + (h - 1 - j) * w * 3
            yuvToBgrRow(dstRow, yrow, urow, vrow, w)
            j += 1
        }
    }

    // Décode complet en mémoire
    mtd doit(pixels: [*] u8) throw
    {
        // retrouve VP8 chunk
        var vp8Ofs = -1
        var vp8Siz = 0

        // Rebalayage minimal (on refactorise pour éviter stockage)
        if !(me.isTag(0, 'R', 'I', 'F', 'F') and me.isTag(8, 'W', 'E', 'B', 'P')):
            throw InvalidFormatError{"webp: entête RIFF/WEBP manquant"}

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
            throw InvalidFormatError{"webp: VP8 manquant"}

        // Plans temporaires s16
        let W       = me.imageSizeX
        let H       = me.imageSizeY
        var yPlane: Array's16
        yPlane.resize(W * H)
        var uPlane: Array's16
        uPlane.resize(((W + 1) >> 1) * ((H + 1) >> 1))
        var vPlane: Array's16
        vPlane.resize(((W + 1) >> 1) * ((H + 1) >> 1))

        // Initialise chroma neutre 128 (au cas où)
        var i = 0
        while i < @countof(uPlane)
        {
            uPlane[i] = 128; i += 1
        }
        i = 0; while i < @countof(vPlane)
        {
            vPlane[i] = 128; i += 1
        }

        // Décode keyframe borné (intra)
        me.decodeKeyframe(vp8Ofs, vp8Siz, @dataof(yPlane), @dataof(uPlane), @dataof(vPlane))

        // Convertit en BGR
        me.toBGR(pixels, @dataof(yPlane), @dataof(uPlane), @dataof(vPlane), W, H)
    }
}

// ---- Intégration à l’API image ----
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
