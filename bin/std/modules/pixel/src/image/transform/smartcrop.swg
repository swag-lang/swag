using Core

public impl Image
{
    private struct SaliencyWeights
    {
        contrast:       f32
        saturation:     f32
        edge:           f32
    }

    private struct ScoreCtx
    {
        iiS:         *Array'f32     // Integral image of saliency values (blurred/combined)
        iiT:         *Array'f32     // Integral image of rule-of-thirds weights
        iiE:         *Array'f32     // Integral image of edge distance values (high away from edges)
        w, h:        s32
        imgArea:     f32
        thirdsW:     f32
        centerW:     f32
        shrinkW:     f32
        edgePen:     f32
        cx, cy:      f32
        d2Max:       f32            // (w/2)^2 + (h/2)^2
    }

    // Pack of precomputed features + integrals to avoid recomputation
    private struct FeaturePack
    {
        sblur:     Array'f32     // blurred saliency (and may contain thirds blended in fast paths)
        T:         Array'f32     // thirds map (may be zero if disabled)
        E:         Array'f32     // edge distance map (high away from edges; 1 at far, 0 at edges)
        iiS:       Array'f32
        iiT:       Array'f32
        iiE:       Array'f32
    }

    private const SALIENCY_RADIUS = 4
    private const THIRDS_SIGMA = 0.075
    private const EDGE_DIST_SCALE = 0.15
    private const ENHANCED_WEIGHTS = SaliencyWeights{contrast: 1.0, saturation: 0.55, edge: 1.2}

    #[Swag.Inline]
    private func srgbToLinear(c: f32)->f32
    {
        if c <= 0.04045 do
            return c / 12.92
        return Math.pow((c + 0.055) / 1.055, 2.4)
    }

    private func buildIntegral(src: Array'f32, w, h: s32)->Array'f32
    {
        var ii: retval
        ii.resize((w + 1) * (h + 1))
        let pitch = w + 1
        for y in 1 to h
        {
            var rowSum  = 0.0
            let baseIn  = (y - 1) * w
            let baseOut = y * pitch
            for x in 1 to w
            {
                rowSum += src[baseIn + (x - 1)]
                ii[baseOut + x] = ii[baseOut - pitch + x] + rowSum
            }
        }
        return ii
    }

    #[Swag.Inline]
    private func sumRect(ii: &Array'f32, w, h, x0, y0, x1, y1: s32)->f32
    {
        let a = y0 * (w + 1) + x0
        let b = y0 * (w + 1) + x1
        let c = y1 * (w + 1) + x0
        let d = y1 * (w + 1) + x1
        return ii[d] - ii[b] - ii[c] + ii[a]
    }

    private func boxBlurViaIntegral(src: Array'f32, w, h, radius: s32)->Array'f32
    {
        if radius <= 0 do
            return src

        var ii = buildIntegral(src, w, h)

        var dst: retval
        dst.resize(w * h)
        for y in 0 to (h - 1)
        {
            let y0 = Math.max(0, y - radius)
            let y1 = Math.min(h - 1, y + radius)
            for x in 0 to (w - 1)
            {
                let x0 = Math.max(0, x - radius)
                let x1 = Math.min(w - 1, x + radius)
                let S  = sumRect(&ii, w, h, x0, y0, x1 + 1, y1 + 1)
                let n  = cast(f32) ((x1 - x0 + 1) * (y1 - y0 + 1))
                dst[y * w + x] = cast(f32) (S / n)
            }
        }
        return dst
    }

    private func buildThirdsMap(w, h: s32, sigma: f32)->Array'f32
    {
        var tx, ty: Array'f32
        tx.resize(w)
        ty.resize(h)

        let t1:  f32 = 1.0 / 3.0
        let t2:  f32 = 2.0 / 3.0
        let sig2 = sigma * sigma

        for x in 0 to (w - 1)
        {
            let fx = cast(f32) x / cast(f32) w
            let dx = Math.min(Math.abs(fx - t1), Math.abs(fx - t2))
            tx[x] = Math.exp(-(dx * dx) / (2.0 * sig2))
        }
        for y in 0 to (h - 1)
        {
            let fy = cast(f32) y / cast(f32) h
            let dy = Math.min(Math.abs(fy - t1), Math.abs(fy - t2))
            ty[y] = Math.exp(-(dy * dy) / (2.0 * sig2))
        }

        var T: retval
        T.resize(w * h)
        for y in 0 to (h - 1) do
            for x in 0 to (w - 1) do
                T[y * w + x] = Math.pow(tx[x] * ty[y], 1.5)
        return T
    }

    // Sobel on (slightly) blurred luminance (shared by saliency and edge-distance)
    private func sobelMagFromLumBlur(lumBlur: &Array'f32, w, h: s32, outEdge: &Array'f32)
    {
        for y in 0 to (h - 1)
        {
            for x in 0 to (w - 1)
            {
                let i = y * w + x
                if x == 0 or y == 0 or x == w - 1 or y == h - 1
                {
                    outEdge[i] = 0.0
                    continue
                }

                let Lm1m1 = lumBlur[((y - 1) * w + (x - 1))]
                let Lp1m1 = lumBlur[((y - 1) * w + (x + 1))]
                let Lm10  = lumBlur[(y * w + (x - 1))]
                let Lp10  = lumBlur[(y * w + (x + 1))]
                let Lm1p1 = lumBlur[((y + 1) * w + (x - 1))]
                let Lp1p1 = lumBlur[((y + 1) * w + (x + 1))]
                let L0m1  = lumBlur[((y - 1) * w + x)]
                let L0p1  = lumBlur[((y + 1) * w + x)]

                let gx = -Lm1m1 + Lp1m1 - 2.0 * Lm10 + 2.0 * Lp10 - Lm1p1 + Lp1p1
                let gy = -Lm1m1 - 2.0 * L0m1 - Lp1m1 + Lm1p1 + 2.0 * L0p1 + Lp1p1
                outEdge[i] = cast(f32) Math.sqrt(gx * gx + gy * gy)
            }
        }
    }

    // Build luminance & saturation once (used by saliency and, indirectly, edge)
    private mtd const extractLumSat(lum: &Array'f32, sat: &Array'f32)
    {
        var f   = {lum: lum, sat: sat}
        let mec = cast #unconst (*Image) me
        mec.visitPixels(&f)
        {
            let p      = image.getPixelColor(x, y)
            let params = cast(*#decltype(f)) userData

            // linear luminance
            params.lum.buffer[index] = p.getRelativeLuminance()

            // saturation in sRGB
            let rs   = p.r / 255.0
            let gs   = p.g / 255.0
            let bs   = p.b / 255.0
            let maxC = Math.max(Math.max(rs, gs), bs)
            let minC = Math.min(Math.min(rs, gs), bs)
            let eps  = 1e-6'f32
            params.sat.buffer[index] = (maxC > 0.0) ? ((maxC - minC) / (maxC + eps)) : 0.0
        }
    }

    // Correct edge distance map: high away from edges, low near edges
    private func buildEdgeDistanceMap(edges: &Array'f32, w, h: s32)->Array'f32
    {
        // blur edge magnitude
        let B = boxBlurViaIntegral(edges, w, h, 3)

        // invert & gently sharpen far-from-edge regions
        var D: retval
        D.resize(w * h)
        for i in 0 to (w * h - 1)
        {
            let e = Math.saturate(B[i])
            // either power inversion:
            let inv = 1.0 - e
            D[i] = Math.pow(inv, 0.75)
            // or exponential alternative (commented):
            // D[i] = Math.exp(-EDGE_DIST_SCALE * e)
        }
        return D
    }

    private func estimatePercentiles(buf: &Array'f32, N: s32, pLow, pHigh: f32)->{ a: f32, b: f32 }
    {
        const K  = 4096's32
        var tmp: Array'f32
        var M    = Math.min(K, N)
        tmp.resize(M)

        if N <= K
        {
            for i in 0 to (N - 1) do
                tmp[i] = buf[i]
        }
        else
        {
            let step = cast(s32) Math.floor(cast(f32) N / M)
            var w    = 0's32
            for var i = 0; i < N; i += step
            {
                if w >= M do
                    break
                tmp[w] = buf[i]
                w += 1
            }

            M = w
        }

        tmp.sort()

        let il = Math.max(0, Math.min(M - 1, cast(s32) Math.floor(pLow * cast(f32) (M - 1))))
        let ih = Math.max(0, Math.min(M - 1, cast(s32) Math.floor(pHigh * cast(f32) (M - 1))))
        let lo = tmp[il]
        let hi = tmp[ih]
        if hi <= lo do
            return {lo, lo + 1e-6}
        return {lo, hi}
    }

    // Core saliency builder that reuses extracted lum/sat and Sobel edges
    private mtd const buildSaliencyAndEdges(w, h: s32, weights: SaliencyWeights)->{ S: Array'f32, Eedge: Array'f32 }
    {
        let N = cast(s32) (w * h)

        var lum, sat: Array'f32
        lum.resize(N)
        sat.resize(N)
        .extractLumSat(&lum, &sat)

        // integral images of luminance for local contrast
        var ii, iiSq: Array'f32
        ii.resize((w + 1) * (h + 1))
        iiSq.resize((w + 1) * (h + 1))
        for y in 1 to h
        {
            var rowSum   = 0.0
            var rowSumSq = 0.0
            for x in 1 to w
            {
                let Ld = cast(f32) lum[(y - 1) * w + (x - 1)]
                rowSum += Ld
                rowSumSq += Ld * Ld
                let idxI = (y * (w + 1) + x)
                ii[idxI]   = ii[idxI - (w + 1)] + rowSum
                iiSq[idxI] = iiSq[idxI - (w + 1)] + rowSumSq
            }
        }

        // Blur luminance and compute Sobel once
        var lumBlur  = boxBlurViaIntegral(&lum, w, h, 1)
        var edgeMag: Array'f32
        edgeMag.resize(N)
        sobelMagFromLumBlur(&lumBlur, w, h, &edgeMag)

        // Downsampled local contrast (same as before)
        const R       = SALIENCY_RADIUS
        var contrast: Array'f32
        contrast.resize(N)
        let sampleStep = 2
        for var y = 0; y < h; y += sampleStep
        {
            for var x = 0; x < w; x += sampleStep
            {
                let i  = y * w + x
                let x0 = Math.max(0, x - R)
                let y0 = Math.max(0, y - R)
                let x1 = Math.min(w - 1, x + R)
                let y1 = Math.min(h - 1, y + R)

                let A = y0 * (w + 1) + x0
                let B = y0 * (w + 1) + (x1 + 1)
                let C = (y1 + 1) * (w + 1) + x0
                let D = (y1 + 1) * (w + 1) + (x1 + 1)

                let n     = cast(f32) ((x1 - x0 + 1) * (y1 - y0 + 1))
                let sum   = ii[D] - ii[B] - ii[C] + ii[A]
                let sumSq = iiSq[D] - iiSq[B] - iiSq[C] + iiSq[A]

                let mean = sum / n
                let ovar = Math.max(0.0, (sumSq / n) - mean * mean)
                contrast[i] = cast(f32) Math.sqrt(ovar)

                if x + 1 < w do
                    contrast[i + 1] = contrast[i]
                if y + 1 < h
                {
                    contrast[i + w] = contrast[i]
                    if x + 1 < w do
                        contrast[i + w + 1] = contrast[i]
                }
            }
        }

        // Normalize features, then combine
        let (cLo, cHi) = estimatePercentiles(&contrast, N, 0.01, 0.99)
        let (sLo, sHi) = estimatePercentiles(&sat, N, 0.01, 0.99)
        let (eLo, eHi) = estimatePercentiles(&edgeMag, N, 0.01, 0.99)

        let cDen = Math.max(1e-6, cHi - cLo)
        let sDen = Math.max(1e-6, sHi - sLo)
        let eDen = Math.max(1e-6, eHi - eLo)

        var S: Array'f32
        S.resize(N)
        for i in 0 to (N - 1)
        {
            let cn = Math.saturate((contrast[i] - cLo) / cDen)
            let sn = Math.saturate((sat[i] - sLo) / sDen)
            let en = Math.saturate((edgeMag[i] - eLo) / eDen)

            let combined = weights.contrast * Math.pow(cn, 0.9) +
                           weights.saturation * Math.pow(sn, 1.1) +
                           weights.edge * Math.pow(en, 0.85)
            S[i] = combined
        }

        return {S, edgeMag}
    }

    private func getThirdsOrZero(w, h: s32, enabled: bool)->Array'f32
    {
        var tmp: retval
        tmp.resize(w * h)
        if enabled do
            return buildThirdsMap(w, h, THIRDS_SIGMA)
        return tmp
    }

    private mtd const buildFeaturePack(w, h: s32, opts: SmartCropSearchOpts, needEdges: bool)->FeaturePack
    {
        // Build saliency and edge magnitude once
        var (S, Eedge) = .buildSaliencyAndEdges(w, h, ENHANCED_WEIGHTS)

        // Blur saliency
        var sb = boxBlurViaIntegral(&S, w, h, opts.blurRadius)

        // Thirds map (or zero)
        var T = getThirdsOrZero(w, h, opts.thirdsWeight > 0.01)

        // Edge distance (or ones if disabled)
        var E: Array'f32
        if needEdges and opts.edgePenalty > 0.01
        {
            E = buildEdgeDistanceMap(&Eedge, w, h)
        }
        else
        {
            E.resize(w * h)
            for i in 0 to (w * h - 1) do
                E[i] = 1.0
        }

        // Build integrals
        let iiS = buildIntegral(&sb, w, h)
        let iiT = buildIntegral(&T, w, h)
        let iiE = buildIntegral(&E, w, h)

        var pack: retval
        pack.sblur = sb
        pack.E     = E
        pack.T     = T
        pack.iiS   = iiS
        pack.iiT   = iiT
        pack.iiE   = iiE
        return pack
    }

    private func prepareScoreCtx(w, h: s32, pack: &FeaturePack, imgArea: f32, opts: SmartCropSearchOpts, shrinkW: f32)->ScoreCtx
    {
        let cx   = w * 0.5
        let cy   = h * 0.5
        let d2Mx = cx * cx + cy * cy     // correct normalization

        var ctx: retval
        ctx.iiS     = &pack.iiS
        ctx.iiT     = &pack.iiT
        ctx.iiE     = &pack.iiE
        ctx.w       = w
        ctx.h       = h
        ctx.imgArea = imgArea
        ctx.thirdsW = opts.thirdsWeight
        ctx.centerW = opts.centerWeight
        ctx.shrinkW = shrinkW
        ctx.edgePen = opts.edgePenalty
        ctx.cx      = cx
        ctx.cy      = cy
        ctx.d2Max   = d2Mx
        return ctx
    }

    private func scoreWindow(ctx: &ScoreCtx, x0, y0, cw, ch: s32)->{ score: f32, d2: f32 }
    {
        let x1   = x0 + cw
        let y1   = y0 + ch
        let area = cast(f32) (cw * ch)

        let sumS = sumRect(ctx.iiS, ctx.w, ctx.h, x0, y0, x1, y1)
        let sumT = sumRect(ctx.iiT, ctx.w, ctx.h, x0, y0, x1, y1)
        let sumE = sumRect(ctx.iiE, ctx.w, ctx.h, x0, y0, x1, y1)

        var s = (sumS / area) + ctx.thirdsW * (sumT / area)

        // Edge penalty: E is high away from edges, low at edges
        s -= ctx.edgePen * (1.0 - (sumE / area))

        // Shrink prior should favor larger crops
        let areaFrac = area / ctx.imgArea
        s += ctx.shrinkW * areaFrac

        // Center bonus (normalized by corrected d2Max)
        let cwx = x0 + 0.5 * cw
        let cwy = y0 + 0.5 * ch
        let d2  = (cwx - ctx.cx) * (cwx - ctx.cx) + (cwy - ctx.cy) * (cwy - ctx.cy)
        s += ctx.centerW * (1.0 - (d2 / ctx.d2Max))

        return {s, d2}
    }

    private func fitAspect(cw, ch, w, h: s32, aspect, imgAspect: f32)->{ w0: s32, h0: s32 }
    {
        var w0 = cw, h0 = ch
        if imgAspect > aspect
        {
            h0 = Math.min(h, Math.max(4, h0))
            w0 = Math.min(w, Math.max(4, Math.roundS32(cast(f32) h0 * aspect)))
        }
        else
        {
            w0 = Math.min(w, Math.max(4, w0))
            h0 = Math.min(h, Math.max(4, Math.roundS32(cast(f32) w0 / aspect)))
        }

        return {w0, h0}
    }

    private func refinePosition(ctx: &ScoreCtx, bestX, bestY, bestW, bestH: s32, range: s32 = 2)->{ x: s32, y: s32, score: f32, d2: f32 }
    {
        let maxX   = ctx.w - bestW
        let maxY   = ctx.h - bestH
        var bx = bestX, by = bestY
        var bScore = -1.0e30
        var bD2    = 1.0e30

        let r0 = scoreWindow(ctx, bx, by, bestW, bestH)
        bScore = r0.score; bD2    = r0.d2

        let rx0 = Math.clamp(bestX - range, 0, maxX)
        let ry0 = Math.clamp(bestY - range, 0, maxY)
        let rx1 = Math.clamp(bestX + range, 0, maxX)
        let ry1 = Math.clamp(bestY + range, 0, maxY)

        for y0 in ry0 to ry1
        {
            for x0 in rx0 to rx1
            {
                let r = scoreWindow(ctx, x0, y0, bestW, bestH)
                if r.score > bScore + Math.ConstF32.Epsilon or (Math.abs(r.score - bScore) <= Math.ConstF32.Epsilon and r.d2 < bD2)
                {
                    bx     = x0
                    by     = y0
                    bScore = r.score
                    bD2    = r.d2
                }
            }
        }
        return {bx, by, bScore, bD2}
    }

    private func tryScaledVariants(ctx: &ScoreCtx, aspect, imgAspect: f32, bestX, bestY, bestW, bestH: s32)->{ x: s32, y: s32, w: s32, h: s32, score: f32, d2: f32 }
    {
        let scales: [5] f32 = [0.88, 0.94, 1.00, 1.06, 1.12]

        var bx = bestX, by = bestY, bw = bestW, bh = bestH
        var bScore = -1.0e30
        var bD2    = 1.0e30

        {
            let r0 = scoreWindow(ctx, bestX, bestY, bestW, bestH)
            bScore = r0.score; bD2    = r0.d2
        }

        for i in 0 to 4
        {
            if i == 2 do
                continue
            let cw = Math.roundS32(bestW * scales[i])
            let ch = Math.roundS32(bestH * scales[i])
            if cw < 4 or ch < 4 do
                continue
            if cw > ctx.w or ch > ctx.h do
                continue

            let (fw, fh) = fitAspect(cw, ch, ctx.w, ctx.h, aspect, imgAspect)
            if fw < 4 or fh < 4 do
                continue

            let maxX = ctx.w - fw
            let maxY = ctx.h - fh
            if maxX < 0 or maxY < 0 do
                continue

            let x0 = Math.clamp(bestX + (bestW - fw) / 2, 0, maxX)
            let y0 = Math.clamp(bestY + (bestH - fh) / 2, 0, maxY)
            let r  = scoreWindow(ctx, x0, y0, fw, fh)

            if r.score > bScore + Math.ConstF32.Epsilon or (Math.abs(r.score - bScore) <= Math.ConstF32.Epsilon and r.d2 < bD2)
            {
                bx     = x0
                by     = y0
                bw     = fw
                bh     = fh
                bScore = r.score
                bD2    = r.d2
            }
        }

        return {bx, by, bw, bh, bScore, bD2}
    }

    private mtd const calculateSingleScale(aspect, imgAspect: f32, w, h: s32, opts: SmartCropSearchOpts)->Math.Rectangle
    {
        var cropW: s32
        var cropH: s32
        if imgAspect > aspect
        {
            cropH = h
            cropW = cast(s32) Math.floor(aspect * cast(f32) cropH + 0.5)
        }
        else
        {
            cropW = w
            cropH = cast(s32) Math.floor(cast(f32) cropW / aspect + 0.5)
        }

        // Features (no shrink prior, still want edges if edgePenalty>0)
        var pack = .buildFeaturePack(w, h, opts, (opts.edgePenalty > 0.01))

        let imgArea = cast(f32) (w * h)
        var ctx     = prepareScoreCtx(w, h, &pack, imgArea, opts, 0.0)     // single-scale ignores shrink prior

        var bestX, bestY = 0
        var bestScore = -1.0e30
        var bestD2    = 1.0e30

        let maxX   = w - cropW
        let maxY   = h - cropH
        let stride = Math.max(1, Math.roundS32(opts.strideFrac * cast(f32) Math.min(cropW, cropH)))

        for var y0 = 0; y0 <= maxY; y0 += stride
        {
            for var x0 = 0; x0 <= maxX; x0 += stride
            {
                let result = scoreWindow(&ctx, x0, y0, cropW, cropH)
                if result.score > bestScore + Math.ConstF32.Epsilon or (Math.abs(result.score - bestScore) <= Math.ConstF32.Epsilon and result.d2 < bestD2)
                {
                    bestScore = result.score
                    bestX     = x0
                    bestY     = y0
                    bestD2    = result.d2
                }
            }
        }

        let refineRange = Math.max(1, Math.min(3, stride / 2))
        let rx0         = Math.clamp(bestX - refineRange, 0, maxX)
        let ry0         = Math.clamp(bestY - refineRange, 0, maxY)
        let rx1         = Math.clamp(bestX + refineRange, 0, maxX)
        let ry1         = Math.clamp(bestY + refineRange, 0, maxY)
        for y0 in ry0 to ry1
        {
            for x0 in rx0 to rx1
            {
                let result = scoreWindow(&ctx, x0, y0, cropW, cropH)
                if result.score > bestScore + Math.ConstF32.Epsilon
                {
                    bestScore = result.score
                    bestX     = x0
                    bestY     = y0
                }
            }
        }

        var r: retval
        r.x      = bestX
        r.y      = bestY
        r.width  = cropW
        r.height = cropH
        return r
    }

    private mtd const calculateMultiScale(aspect, imgAspect: f32, w, h: s32, opts: SmartCropSearchOpts)->Math.Rectangle
    {
        // Full feature pack (with edges if enabled)
        var pack = .buildFeaturePack(w, h, opts, true)

        let imgArea = cast(f32) (w * h)
        var ctx     = prepareScoreCtx(w, h, &pack, imgArea, opts, cast(f32) opts.shrinkPrior)

        let minArea = opts.minCoverage * imgArea
        let steps   = opts.maxSearchSteps

        var bestX, bestY, bestW, bestH = 0
        var bestScore    = -1.0e30
        var bestCenterD2 = 1.0e30

        for step in 0 to (steps - 1)
        {
            let t          = step / cast(f32) (steps - 1)
            let frac       = 1.0 - Math.pow(t, 1.3) * (1.0 - opts.minCoverage)
            let targetArea = Math.max(frac * imgArea, minArea)

            var cw, ch: s32
            if imgAspect > aspect
            {
                ch = Math.min(h, Math.max(8, Math.roundS32(Math.sqrt(targetArea / aspect))))
                cw = Math.min(w, Math.roundS32(ch * aspect))
            }
            else
            {
                cw = Math.min(w, Math.max(8, Math.roundS32(Math.sqrt(targetArea * aspect))))
                ch = Math.min(h, Math.roundS32(cw / aspect))
            }

            if cw < 4 or ch < 4 do
                continue
            let maxX = w - cw
            let maxY = h - ch
            if maxX < 0 or maxY < 0 do
                continue

            let strideX = Math.max(1, Math.roundS32(opts.strideFrac * cw))
            let strideY = Math.max(1, Math.roundS32(opts.strideFrac * ch))

            for var y0 = 0; y0 <= maxY; y0 += strideY
            {
                for var x0 = 0; x0 <= maxX; x0 += strideX
                {
                    let r = scoreWindow(&ctx, x0, y0, cw, ch)
                    if r.score > bestScore + Math.ConstF32.Epsilon or (Math.abs(r.score - bestScore) <= Math.ConstF32.Epsilon and r.d2 < bestCenterD2)
                    {
                        bestScore    = r.score
                        bestX        = x0
                        bestY        = y0
                        bestW        = cw
                        bestH        = ch
                        bestCenterD2 = r.d2
                    }
                }
            }
        }

        if bestW == 0 or bestH == 0
        {
            var rf: retval
            rf.x      = 0
            rf.y      = 0
            rf.width  = w
            rf.height = h
            return rf
        }

        {
            let rr = refinePosition(&ctx, bestX, bestY, bestW, bestH, 2)
            if rr.score > bestScore + Math.ConstF32.Epsilon
            {
                bestScore    = rr.score
                bestX        = rr.x
                bestY        = rr.y
                bestCenterD2 = rr.d2
            }

            let sr = tryScaledVariants(&ctx, aspect, imgAspect, bestX, bestY, bestW, bestH)
            if sr.score > bestScore + Math.ConstF32.Epsilon
            {
                bestScore    = sr.score
                bestX        = sr.x
                bestY        = sr.y
                bestW        = sr.w
                bestH        = sr.h
                bestCenterD2 = sr.d2
            }

            let rr2 = refinePosition(&ctx, bestX, bestY, bestW, bestH, 1)
            if rr2.score > bestScore + Math.ConstF32.Epsilon
            {
                bestX = rr2.x
                bestY = rr2.y
            }
        }

        var r: retval
        r.x      = bestX
        r.y      = bestY
        r.width  = bestW
        r.height = bestH
        return r
    }

    private mtd const calculateReframeRect(targetWidth, targetHeight: f32, opts: SmartCropSearchOpts)->Math.Rectangle
    {
        let w         = .width
        let h         = .height
        let aspect    = targetWidth / targetHeight
        let imgAspect = cast(f32) w / h

        if opts.minCoverage >= 0.95 do
            return .calculateSingleScale(aspect, imgAspect, w, h, opts)
        return .calculateMultiScale(aspect, imgAspect, w, h, opts)
    }

    private mtd const evaluateCropScoreFast(rect: Math.Rectangle, opts: SmartCropSearchOpts)->f32
    {
        let w = .width, h = .height

        // No edge penalty, light-third blending if requested
        var pack = .buildFeaturePack(w, h, opts, false)

        if opts.thirdsWeight > 0.01
        {
            for i in 0 until w * h
            {
                pack.sblur[i] = pack.sblur[i] + opts.thirdsWeight * pack.T[i]
            }
        }

        var ii   = buildIntegral(&pack.sblur, w, h)
        let area = cast(f32) (rect.width * rect.height)
        let sumS = sumRect(&ii, w, h, cast() rect.x, cast() rect.y, cast() rect.right(), cast() rect.bottom())
        return sumS / area
    }

    private mtd const evaluateCropScoreFull(rect: Math.Rectangle, opts: SmartCropSearchOpts)->f32
    {
        let w = .width, h = .height
        var pack = .buildFeaturePack(w, h, opts, true)
        var ctx  = prepareScoreCtx(w, h, &pack, cast(f32) (w * h), opts, cast(f32) opts.shrinkPrior)
        let r    = scoreWindow(&ctx, cast() rect.x, cast() rect.y, cast() rect.width, cast() rect.height)
        return r.score
    }
}

public impl Image
{
    // Smart crop search parameters
    struct SmartCropSearchOpts
    {
        // Minimum fraction of the original image area the crop must keep.
        // Range: 0.0–1.0. Lower allows tighter crops; higher preserves more context.
        // Special: >= 0.95 triggers fast single-scale mode (no multi-scale search).
        minCoverage: f32 = 0.4

        // Blur radius (in pixels) applied to the saliency map for smoothing.
        // Range: 0–5 (typical 2–4). Higher = smoother gradients, less fine detail.
        blurRadius: s32 = 3

        // Search stride as a fraction of the crop size (per axis).
        // Range: 0.01–0.20 (typical 0.06–0.12). Lower = slower, higher quality.
        strideFrac: f32 = 0.10

        // Weight for the rule-of-thirds compositional bonus.
        // Range: 0.0–0.5 (typical 0.15–0.25). 0.0 disables thirds influence.
        thirdsWeight: f32 = 0.22

        // Weight for proximity to the image center (center bias).
        // Range: 0.0–0.3 (typical 0.05–0.15). 0.0 removes center preference.
        centerWeight: f32 = 0.06

        // Prior favoring larger crops (adds an area-proportional bonus).
        // Range: 0.0–0.3 (typical 0.05–0.15). 0.0 means no size preference.
        shrinkPrior: f32 = 0.08

        // Penalty for crops that cut through strong edges (uses edge-distance map).
        // Range: 0.0–0.5 (typical 0.10–0.25). 0.0 disables the edge term.
        edgePenalty: f32 = 0.15

        // Reserved for future face detection bonus when faces are inside the crop.
        // Range: 0.0–1.0 (currently unused).
        faceWeight: f32 = 0.0

        // Number of scale steps in multi-scale search.
        // Range: 5–20 (typical 8–12). More = more thorough but slower.
        maxSearchSteps: s32 = 10

        // Maximum working image size (pixels) for the longest side when
        // computing crops/scores. 0 (or <=0) disables downscaling.
        // The original image is always used for the final crop.
        maxSize: s32 = 0
    }

    #[Swag.Overload]
    mtd evaluateCropScore(rect: Math.Rectangle, opts: SmartCropSearchOpts = {})->f32
    {
        // If a max working size is set and we’re larger, evaluate on a resized copy
        if opts.maxSize > 0
        {
            let w0      = .width
            let h0      = .height
            let maxSide = Math.max(w0, h0)
            if maxSide > opts.maxSize
            {
                let scale = cast(f32) opts.maxSize / maxSide
                let w2    = Math.max(1, Math.roundS32(scale * w0))
                let h2    = Math.max(1, Math.roundS32(scale * h0))

                var work = dref me
                work.resize(w2, h2)

                // Scale the input rect down to the working copy
                var rs: Math.Rectangle
                rs.x      = Math.clamp(Math.roundS32(rect.x * scale), 0, w2 - 1)
                rs.y      = Math.clamp(Math.roundS32(rect.y * scale), 0, h2 - 1)
                rs.width  = Math.max(1, Math.roundS32(rect.width * scale))
                rs.height = Math.max(1, Math.roundS32(rect.height * scale))
                rs.width  = Math.min(rs.width, w2 - rs.x)
                rs.height = Math.min(rs.height, h2 - rs.y)

                // Evaluate on the working copy (it won’t downscale again since it’s <= maxSize)
                return work.evaluateCropScore(rs, opts)
            }
        }

        // Original (no-downscale) path remains unchanged
        if opts.edgePenalty <= 0.01 and opts.shrinkPrior <= 0.01 and opts.centerWeight <= 0.01 do
            return .evaluateCropScoreFast(rect, opts)
        return .evaluateCropScoreFull(rect, opts)
    }

    mtd smartCrop(rect: Math.Rectangle, opts: SmartCropSearchOpts = {})
    {
        Debug.assert(rect.width > 0 and rect.height > 0)

        let w0 = .width
        let h0 = .height

        // If a max working size is set and we’re larger, compute on a resized copy
        if opts.maxSize > 0
        {
            let maxSide = Math.max(w0, h0)
            if maxSide > opts.maxSize
            {
                let scale = cast(f32) opts.maxSize / maxSide
                let w2    = Math.max(1, Math.roundS32(scale * w0))
                let h2    = Math.max(1, Math.roundS32(scale * h0))

                // Working copy (resized) — use it only to compute the rectangle
                var work = dref me
                work.resize(w2, h2)

                let rSmall = work.calculateReframeRect(rect.width, rect.height, opts)

                // Map the rectangle back to original coordinates
                var r: Math.Rectangle
                r.x      = Math.clamp(Math.roundS32(rSmall.x / scale), 0, w0 - 1)
                r.y      = Math.clamp(Math.roundS32(rSmall.y / scale), 0, h0 - 1)
                r.width  = Math.max(1, Math.roundS32(rSmall.width / scale))
                r.height = Math.max(1, Math.roundS32(rSmall.height / scale))
                r.width  = Math.min(r.width, w0 - r.x)
                r.height = Math.min(r.height, h0 - r.y)

                .crop(r) // Final crop is performed on the ORIGINAL image
                return
            }
        }

        // No downscaling needed — use the existing path
        .crop(.calculateReframeRect(rect.width, rect.height, opts))
    }
}
