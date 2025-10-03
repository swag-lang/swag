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
        d2Max:       f32            // (W/2)^2 + (H/2)^2
    }

    // Pack of precomputed features + integrals to avoid recomputation
    private struct FeaturePack
    {
        Sblur:     Array'f32     // blurred saliency (and may contain thirds blended in fast paths)
        T:         Array'f32     // thirds map (may be zero if disabled)
        E:         Array'f32     // edge distance map (high away from edges; 1 at far, 0 at edges)
        iiS:       Array'f32
        iiT:       Array'f32
        iiE:       Array'f32
    }

    private const SALIENCY_RADIUS = 4
    private const THIRDS_SIGMA = 0.075
    private const EPS_TIE = 1.0e-9'f32
    private const EDGE_DIST_SCALE = 0.15
    private const ENHANCED_WEIGHTS = SaliencyWeights{contrast: 1.0, saturation: 0.55, edge: 1.2}

    #[Swag.Inline]
    private func srgbToLinear(c: f32)->f32
    {
        if c <= 0.04045:
            return c / 12.92
        return Math.pow((c + 0.055) / 1.055, 2.4)
    }

    private func buildIntegral(src: Array'f32, W, H: s32)->Array'f32
    {
        var ii: retval
        ii.resize(cast(u64) ((W + 1) * (H + 1)))

        for y in 1 to H
        {
            var rowSum = 0.0'f32
            for x in 1 to W
            {
                let s = cast(f32) src[(y - 1) * W + (x - 1)]
                rowSum += s
                let idx = (y * (W + 1) + x)
                ii[idx] = ii[idx - (W + 1)] + rowSum
            }
        }

        return ii
    }

    private func sumRect(ii: &Array'f32, W, H, x0, y0, x1, y1: s32)->f32
    {
        let A = y0 * (W + 1) + x0
        let B = y0 * (W + 1) + x1
        let C = y1 * (W + 1) + x0
        let D = y1 * (W + 1) + x1
        return ii[D] - ii[B] - ii[C] + ii[A]
    }

    private func boxBlurViaIntegral(src: Array'f32, W, H, radius: s32)->Array'f32
    {
        if radius <= 0:
            return src

        var ii = buildIntegral(src, W, H)

        var dst: retval
        dst.resize(W * H)
        for y in 0 to (H - 1)
        {
            let y0 = Math.max(0, y - radius)
            let y1 = Math.min(H - 1, y + radius)
            for x in 0 to (W - 1)
            {
                let x0 = Math.max(0, x - radius)
                let x1 = Math.min(W - 1, x + radius)
                let S  = sumRect(&ii, W, H, x0, y0, x1 + 1, y1 + 1)
                let n  = cast(f32) ((x1 - x0 + 1) * (y1 - y0 + 1))
                dst[y * W + x] = cast(f32) (S / n)
            }
        }
        return dst
    }

    private func buildThirdsMap(W, H: s32, sigma: f32)->Array'f32
    {
        var tx, ty: Array'f32
        tx.resize(W)
        ty.resize(H)

        let t1:  f32 = 1.0 / 3.0
        let t2:  f32 = 2.0 / 3.0
        let sig2 = sigma * sigma

        for x in 0 to (W - 1)
        {
            let fx = cast(f32) x / cast(f32) W
            let dx = Math.min(Math.abs(fx - t1), Math.abs(fx - t2))
            tx[x] = Math.exp(-(dx * dx) / (2.0 * sig2))
        }
        for y in 0 to (H - 1)
        {
            let fy = cast(f32) y / cast(f32) H
            let dy = Math.min(Math.abs(fy - t1), Math.abs(fy - t2))
            ty[y] = Math.exp(-(dy * dy) / (2.0 * sig2))
        }

        var T: retval
        T.resize(W * H)
        for y in 0 to (H - 1):
            for x in 0 to (W - 1):
                T[y * W + x] = Math.pow(tx[cast(u64) x] * ty[cast(u64) y], 1.5)
        return T
    }

    // Sobel on (slightly) blurred luminance (shared by saliency and edge-distance)
    private func sobelMagFromLumBlur(lumBlur: &Array'f32, W, H: s32, outEdge: &Array'f32)
    {
        for y in 0 to (H - 1)
        {
            for x in 0 to (W - 1)
            {
                let i = cast(u64) (y * W + x)
                if x == 0 or y == 0 or x == W - 1 or y == H - 1
                {
                    outEdge[i] = 0.0
                    continue
                }

                let Lm1m1 = lumBlur[((y - 1) * W + (x - 1))]
                let Lp1m1 = lumBlur[((y - 1) * W + (x + 1))]
                let Lm10  = lumBlur[(y * W + (x - 1))]
                let Lp10  = lumBlur[(y * W + (x + 1))]
                let Lm1p1 = lumBlur[((y + 1) * W + (x - 1))]
                let Lp1p1 = lumBlur[((y + 1) * W + (x + 1))]
                let L0m1  = lumBlur[((y - 1) * W + x)]
                let L0p1  = lumBlur[((y + 1) * W + x)]

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
            let p = image.getPixelColor(x, y)

            // linear luminance
            let r_lin  = srgbToLinear(p.r / 255.0)
            let g_lin  = srgbToLinear(p.g / 255.0)
            let b_lin  = srgbToLinear(p.b / 255.0)
            let L      = r_lin * 0.2126 + g_lin * 0.7152 + b_lin * 0.0722
            let params = cast(*#decltype(f)) userData
            params.lum.buffer[index] = L

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
    private func buildEdgeDistanceMap(edges: &Array'f32, W, H: s32)->Array'f32
    {
        // blur edge magnitude
        let B = boxBlurViaIntegral(edges, W, H, 3)

        // invert & gently sharpen far-from-edge regions
        var D: retval
        D.resize(W * H)
        for i in 0 to (W * H - 1)
        {
            let e = Math.saturate(B[cast(u64) i])
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
            for i in 0 to (N - 1):
                tmp[i] = buf[i]
        }
        else
        {
            let step = cast(s32) Math.floor(cast(f32) N / M)
            var w    = 0's32
            for var i = 0; i < N; i += step
            {
                if w >= M:
                    break
                tmp[w] = buf[i]
                w += 1
            }

            M = w
        }

        tmp.sort()

        let il = Math.max(0, Math.min(M - 1, cast(s32) Math.floor(pLow * cast(f32) (M - 1))))
        let ih = Math.max(0, Math.min(M - 1, cast(s32) Math.floor(pHigh * cast(f32) (M - 1))))
        let lo = tmp[cast(u64) il]
        let hi = tmp[cast(u64) ih]
        if hi <= lo:
            return {lo, lo + 1e-6}
        return {lo, hi}
    }

    // Core saliency builder that reuses extracted lum/sat and Sobel edges
    private mtd const buildSaliencyAndEdges(W, H: s32, weights: SaliencyWeights)->{ S: Array'f32, Eedge: Array'f32 }
    {
        let N = cast(s32) (W * H)

        var lum, sat: Array'f32
        lum.resize(N)
        sat.resize(N)
        .extractLumSat(&lum, &sat)

        // integral images of luminance for local contrast
        var ii, iiSq: Array'f32
        ii.resize((W + 1) * (H + 1))
        iiSq.resize((W + 1) * (H + 1))
        for y in 1 to H
        {
            var rowSum   = 0.0
            var rowSumSq = 0.0
            for x in 1 to W
            {
                let Ld = cast(f32) lum[(y - 1) * W + (x - 1)]
                rowSum += Ld
                rowSumSq += Ld * Ld
                let idxI = (y * (W + 1) + x)
                ii[idxI]   = ii[idxI - (W + 1)] + rowSum
                iiSq[idxI] = iiSq[idxI - (W + 1)] + rowSumSq
            }
        }

        // Blur luminance and compute Sobel once
        var lumBlur  = boxBlurViaIntegral(&lum, W, H, 1)
        var edgeMag: Array'f32
        edgeMag.resize(cast(u64) N)
        sobelMagFromLumBlur(&lumBlur, W, H, &edgeMag)

        // Downsampled local contrast (same as before)
        const R       = SALIENCY_RADIUS
        var contrast: Array'f32
        contrast.resize(cast(u64) N)
        let sampleStep = 2
        for var y = 0; y < H; y += sampleStep
        {
            for var x = 0; x < W; x += sampleStep
            {
                let i  = y * W + x
                let x0 = Math.max(0, x - R)
                let y0 = Math.max(0, y - R)
                let x1 = Math.min(W - 1, x + R)
                let y1 = Math.min(H - 1, y + R)

                let A = y0 * (W + 1) + x0
                let B = y0 * (W + 1) + (x1 + 1)
                let C = (y1 + 1) * (W + 1) + x0
                let D = (y1 + 1) * (W + 1) + (x1 + 1)

                let n     = cast(f32) ((x1 - x0 + 1) * (y1 - y0 + 1))
                let sum   = ii[D] - ii[B] - ii[C] + ii[A]
                let sumSq = iiSq[D] - iiSq[B] - iiSq[C] + iiSq[A]

                let mean = sum / n
                let ovar = Math.max(0.0, (sumSq / n) - mean * mean)
                contrast[i] = cast(f32) Math.sqrt(ovar)

                if x + 1 < W:
                    contrast[i + 1] = contrast[i]
                if y + 1 < H
                {
                    contrast[i + W] = contrast[i]
                    if x + 1 < W:
                        contrast[i + W + 1] = contrast[i]
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

        if enabled:
            return buildThirdsMap(w, h, THIRDS_SIGMA)

        tmp.fill(0)
        return tmp
    }

    private mtd const buildFeaturePack(W, H: s32, opts: SmartCropSearchOpts, needEdges: bool)->FeaturePack
    {
        // Build saliency and edge magnitude once
        var (S, Eedge) = .buildSaliencyAndEdges(W, H, ENHANCED_WEIGHTS)

        // Blur saliency
        var Sb = boxBlurViaIntegral(&S, W, H, opts.blurRadius)

        // Thirds map (or zero)
        var T = getThirdsOrZero(W, H, opts.thirdsWeight > 0.01)

        // Edge distance (or ones if disabled)
        var E: Array'f32
        if needEdges and opts.edgePenalty > 0.01
        {
            E = buildEdgeDistanceMap(&Eedge, W, H)
        }
        else
        {
            E.resize(cast(u64) (W * H))
            for i in 0 to (W * H - 1):
                E[cast(u64) i] = 1.0
        }

        // Build integrals
        let iiS = buildIntegral(&Sb, W, H)
        let iiT = buildIntegral(&T, W, H)
        let iiE = buildIntegral(&E, W, H)

        var pack: retval
        pack.Sblur = Sb
        pack.E     = E
        pack.T     = T
        pack.iiS   = iiS
        pack.iiT   = iiT
        pack.iiE   = iiE
        return pack
    }

    private func prepareScoreCtx(W, H: s32, pack: &FeaturePack, imgArea: f32, opts: SmartCropSearchOpts, shrinkW: f32)->ScoreCtx
    {
        let cx   = 0.5'f32 * cast(f32) W
        let cy   = 0.5'f32 * cast(f32) H
        let d2Mx = cx * cx + cy * cy         // correct normalization

        var ctx: ScoreCtx
        ctx.iiS     = &pack.iiS
        ctx.iiT     = &pack.iiT
        ctx.iiE     = &pack.iiE
        ctx.w       = W
        ctx.h       = H
        ctx.imgArea = imgArea
        ctx.thirdsW = cast(f32) opts.thirdsWeight
        ctx.centerW = cast(f32) opts.centerWeight
        ctx.shrinkW = shrinkW
        ctx.edgePen = cast(f32) opts.edgePenalty
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
        let cwx = cast(f32) x0 + 0.5 * cast(f32) cw
        let cwy = cast(f32) y0 + 0.5 * cast(f32) ch
        let d2  = (cwx - ctx.cx) * (cwx - ctx.cx) + (cwy - ctx.cy) * (cwy - ctx.cy)
        s += ctx.centerW * (1.0 - (d2 / ctx.d2Max))

        return {s, d2}
    }

    private func fitAspect(cw, ch, W, H: s32, aspect, imgAspect: f32)->{ w: s32, h: s32 }
    {
        var w = cw, h = ch
        if imgAspect > aspect
        {
            h = Math.min(H, Math.max(4, h))
            w = Math.min(W, Math.max(4, cast(s32) Math.round(cast(f32) h * cast(f32) aspect)))
        }
        else
        {
            w = Math.min(W, Math.max(4, w))
            h = Math.min(H, Math.max(4, cast(s32) Math.round(cast(f32) w / cast(f32) aspect)))
        }

        return {w, h}
    }

    private func refinePosition(ctx: &ScoreCtx, bestX, bestY, bestW, bestH: s32, range: s32 = 2)->{ x: s32, y: s32, score: f32, d2: f32 }
    {
        let maxX   = ctx.w - bestW
        let maxY   = ctx.h - bestH
        var bx = bestX, by = bestY
        var bScore = -1.0e30'f32
        var bD2    = 1.0e30'f32

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
                if r.score > bScore + EPS_TIE or (Math.abs(r.score - bScore) <= EPS_TIE and r.d2 < bD2)
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
        var bScore = -1.0e30'f32
        var bD2    = 1.0e30'f32

        {
            let r0 = scoreWindow(ctx, bestX, bestY, bestW, bestH)
            bScore = r0.score; bD2    = r0.d2
        }

        for i in 0 to 4
        {
            if i == 2:
                continue
            let cw = cast(s32) Math.round(cast(f32) bestW * scales[i])
            let ch = cast(s32) Math.round(cast(f32) bestH * scales[i])
            if cw < 4 or ch < 4:
                continue
            if cw > ctx.w or ch > ctx.h:
                continue

            let (fw, fh) = fitAspect(cw, ch, ctx.w, ctx.h, aspect, imgAspect)
            if fw < 4 or fh < 4:
                continue

            let maxX = ctx.w - fw
            let maxY = ctx.h - fh
            if maxX < 0 or maxY < 0:
                continue

            let x0 = Math.clamp(bestX + (bestW - fw) / 2, 0, maxX)
            let y0 = Math.clamp(bestY + (bestH - fh) / 2, 0, maxY)
            let r  = scoreWindow(ctx, x0, y0, fw, fh)

            if r.score > bScore + EPS_TIE or (Math.abs(r.score - bScore) <= EPS_TIE and r.d2 < bD2)
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

    private mtd const calculateSingleScale(aspect, imgAspect: f32, W, H: s32, opts: SmartCropSearchOpts)->Math.Rectangle
    {
        var cropW: s32
        var cropH: s32
        if imgAspect > aspect
        {
            cropH = H
            cropW = cast(s32) Math.floor(aspect * cast(f32) cropH + 0.5)
        }
        else
        {
            cropW = W
            cropH = cast(s32) Math.floor(cast(f32) cropW / aspect + 0.5)
        }

        // Features (no shrink prior, still want edges if edgePenalty>0)
        var pack = .buildFeaturePack(W, H, opts, (opts.edgePenalty > 0.01))

        let imgArea = cast(f32) (W * H)
        var ctx     = prepareScoreCtx(W, H, &pack, imgArea, opts, 0.0)     // single-scale ignores shrink prior

        var bestX, bestY = 0's32
        var bestScore = -1.0e30'f32
        var bestD2    = 1.0e30'f32

        let maxX   = W - cropW
        let maxY   = H - cropH
        let stride = Math.max(1, cast(s32) Math.round(opts.strideFrac * cast(f32) Math.min(cropW, cropH)))

        for var y0 = 0; y0 <= maxY; y0 += stride
        {
            for var x0 = 0; x0 <= maxX; x0 += stride
            {
                let result = scoreWindow(&ctx, x0, y0, cropW, cropH)
                if result.score > bestScore + EPS_TIE or (Math.abs(result.score - bestScore) <= EPS_TIE and result.d2 < bestD2)
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
                if result.score > bestScore + EPS_TIE
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

    private mtd const calculateMultiScale(aspect, imgAspect: f32, W, H: s32, opts: SmartCropSearchOpts)->Math.Rectangle
    {
        // Full feature pack (with edges if enabled)
        var pack = .buildFeaturePack(W, H, opts, true)

        let imgArea = cast(f32) (W * H)
        var ctx     = prepareScoreCtx(W, H, &pack, imgArea, opts, cast(f32) opts.shrinkPrior)

        let minArea = cast(f32) opts.minCoverage * imgArea
        let STEPS   = opts.maxSearchSteps

        var bestX, bestY, bestW, bestH = 0's32
        var bestScore    = -1.0e30'f32
        var bestCenterD2 = 1.0e30'f32

        for step in 0 to (STEPS - 1)
        {
            let t          = cast(f32) step / cast(f32) (STEPS - 1)
            let frac       = 1.0 - Math.pow(t, 1.3) * (1.0 - cast(f32) opts.minCoverage)
            let targetArea = Math.max(frac * imgArea, minArea)

            var cw, ch: s32
            if imgAspect > aspect
            {
                ch = Math.min(H, Math.max(8, cast(s32) Math.round(Math.sqrt(targetArea / cast(f32) aspect))))
                cw = Math.min(W, cast(s32) Math.round(cast(f32) ch * cast(f32) aspect))
            }
            else
            {
                cw = Math.min(W, Math.max(8, cast(s32) Math.round(Math.sqrt(targetArea * cast(f32) aspect))))
                ch = Math.min(H, cast(s32) Math.round(cast(f32) cw / cast(f32) aspect))
            }

            if cw < 4 or ch < 4:
                continue
            let maxX = W - cw
            let maxY = H - ch
            if maxX < 0 or maxY < 0:
                continue

            let strideX = Math.max(1, cast(s32) Math.round(opts.strideFrac * cast(f32) cw))
            let strideY = Math.max(1, cast(s32) Math.round(opts.strideFrac * cast(f32) ch))

            for var y0 = 0; y0 <= maxY; y0 += strideY
            {
                for var x0 = 0; x0 <= maxX; x0 += strideX
                {
                    let r = scoreWindow(&ctx, x0, y0, cw, ch)
                    if r.score > bestScore + EPS_TIE or (Math.abs(r.score - bestScore) <= EPS_TIE and r.d2 < bestCenterD2)
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
            rf.x      = 0; rf.y      = 0; rf.width  = W; rf.height = H
            return rf
        }

        {
            let rr = refinePosition(&ctx, bestX, bestY, bestW, bestH, 2)
            if rr.score > bestScore + EPS_TIE
            {
                bestScore    = rr.score
                bestX        = rr.x
                bestY        = rr.y
                bestCenterD2 = rr.d2
            }

            let sr = tryScaledVariants(&ctx, aspect, imgAspect, bestX, bestY, bestW, bestH)
            if sr.score > bestScore + EPS_TIE
            {
                bestScore    = sr.score
                bestX        = sr.x
                bestY        = sr.y
                bestW        = sr.w
                bestH        = sr.h
                bestCenterD2 = sr.d2
            }

            let rr2 = refinePosition(&ctx, bestX, bestY, bestW, bestH, 1)
            if rr2.score > bestScore + EPS_TIE
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
        let W = .width, H = .height
        Debug.assert(W > 0 and H > 0)

        let aspect    = targetWidth / targetHeight
        let imgAspect = cast(f32) W / cast(f32) H

        if opts.minCoverage >= 0.95:
            return .calculateSingleScale(aspect, imgAspect, W, H, opts)
        return .calculateMultiScale(aspect, imgAspect, W, H, opts)
    }

    private mtd const evaluateCropScoreFast(rect: Math.Rectangle, opts: SmartCropSearchOpts)->f32
    {
        let W = .width, H = .height

        // No edge penalty, light-third blending if requested
        var pack = .buildFeaturePack(W, H, opts, false)

        if opts.thirdsWeight > 0.01
        {
            for i in 0 to (W * H - 1)
            {
                let idx = cast(u64) i
                pack.Sblur[idx] = pack.Sblur[idx] + opts.thirdsWeight * pack.T[idx]
            }
        }

        var ii   = buildIntegral(&pack.Sblur, W, H)
        let area = cast(f32) (rect.width * rect.height)
        let sumS = sumRect(&ii, W, H, cast() rect.x, cast() rect.y, cast() rect.right(), cast() rect.bottom())
        return sumS / area
    }

    private mtd const evaluateCropScoreFull(rect: Math.Rectangle, opts: SmartCropSearchOpts)->f32
    {
        let W = .width, H = .height
        var pack = .buildFeaturePack(W, H, opts, true)

        let imgArea = cast(f32) (W * H)
        var ctx     = prepareScoreCtx(W, H, &pack, imgArea, opts, cast(f32) opts.shrinkPrior)

        let r = scoreWindow(&ctx, cast() rect.x, cast() rect.y, cast() rect.width, cast() rect.height)
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
            let W0      = .width
            let H0      = .height
            let maxSide = Math.max(W0, H0)
            if maxSide > opts.maxSize
            {
                let scale = cast(f32) opts.maxSize / maxSide
                let w2    = Math.max(1, cast(s32) Math.round(scale * W0))
                let h2    = Math.max(1, cast(s32) Math.round(scale * H0))

                var work = dref me
                work.resize(w2, h2)

                // Scale the input rect down to the working copy
                var rs: Math.Rectangle
                rs.x      = Math.clamp(cast(s32) Math.round(rect.x * scale), 0, w2 - 1)
                rs.y      = Math.clamp(cast(s32) Math.round(rect.y * scale), 0, h2 - 1)
                rs.width  = Math.max(1, cast(s32) Math.round(rect.width * scale))
                rs.height = Math.max(1, cast(s32) Math.round(rect.height * scale))
                rs.width  = Math.min(rs.width, w2 - rs.x)
                rs.height = Math.min(rs.height, h2 - rs.y)

                // Evaluate on the working copy (it won’t downscale again since it’s <= maxSize)
                return work.evaluateCropScore(rs, opts)
            }
        }

        // Original (no-downscale) path remains unchanged
        if opts.edgePenalty <= 0.01 and opts.shrinkPrior <= 0.01 and opts.centerWeight <= 0.01:
            return .evaluateCropScoreFast(rect, opts)
        return .evaluateCropScoreFull(rect, opts)
    }

    mtd smartCrop(rect: Math.Rectangle, opts: SmartCropSearchOpts = {})
    {
        Debug.assert(rect.width > 0 and rect.height > 0)

        let W0 = .width
        let H0 = .height

        // If a max working size is set and we’re larger, compute on a resized copy
        if opts.maxSize > 0
        {
            let maxSide = Math.max(W0, H0)
            if maxSide > opts.maxSize
            {
                let scale = cast(f32) opts.maxSize / maxSide
                let w2    = Math.max(1, cast(s32) Math.round(scale * W0))
                let h2    = Math.max(1, cast(s32) Math.round(scale * H0))

                // Working copy (resized) — use it only to compute the rectangle
                var work = dref me
                work.resize(w2, h2)

                let rSmall = work.calculateReframeRect(rect.width, rect.height, opts)

                // Map the rectangle back to original coordinates
                var r: Math.Rectangle
                r.x      = Math.clamp(cast(s32) Math.round(rSmall.x / scale), 0, W0 - 1)
                r.y      = Math.clamp(cast(s32) Math.round(rSmall.y / scale), 0, H0 - 1)
                r.width  = Math.max(1, cast(s32) Math.round(rSmall.width / scale))
                r.height = Math.max(1, cast(s32) Math.round(rSmall.height / scale))
                r.width  = Math.min(r.width, W0 - r.x)
                r.height = Math.min(r.height, H0 - r.y)

                .crop(r) // Final crop is performed on the ORIGINAL image
                return
            }
        }

        // No downscaling needed — use the existing path
        .crop(.calculateReframeRect(rect.width, rect.height, opts))
    }
}
