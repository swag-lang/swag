using Core

public impl Image
{
    private struct InterestPoint
    {
        x, y:      f32     // Interest point position (0-1 normalized)
        score:     f32     // Aggregate saliency score at that point
    }

    private struct SaliencyWeights
    {
        contrast:       f32
        saturation:     f32
        edge:           f32
    }

    // Tunables
    private const SALIENCY_RADIUS = 5
    private const THIRDS_SIGMA = 0.08
    private const DEFAULT_WEIGHTS = SaliencyWeights{contrast: 1.0, saturation: 0.65, edge: 0.85}

    // === Utilities ===
    private mtd const clampS32(v, minv, maxv: s32)->s32 { return Math.max(minv, Math.min(maxv, v)); }
    private mtd const clamp01f(v: f32)->f32 { return Math.max(0.0, Math.min(1.0, v)); }

    // sRGB -> linear (approx exact sRGB EOTF)
    private mtd const srgbToLinear(c: f32)->f32
    {
        if c <= 0.04045:
            return c / 12.92
        return Math.pow((c + 0.055) / 1.055, 2.4)
    }

    // === Largest in-bounds crop at fixed aspect, positioned by blurred saliency sum ===
    private mtd const calculateReframeRect(targetWidth, targetHeight: f32)->Math.Rectangle
    {
        let aspect    = targetWidth / targetHeight
        let W         = .width
        let H         = .height
        let imgAspect = cast(f32) W / cast(f32) H

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

        var S: Array'f32
        S.resize(cast(u64) (W * H))
        .computeSaliencyMap(&S, DEFAULT_WEIGHTS)

        const BLUR_RADIUS: s32 = 2 // tiny de-spike

        // integral of original S (with +1 padding)
        var ii0: Array'f64
        ii0.resize(cast(u64) ((W + 1) * (H + 1)))
        for y in 1 to H
        {
            var rowSum = 0.0'f64
            for x in 1 to W
            {
                let s = cast(f64) S[cast(u64) ((y - 1) * W + (x - 1))]
                rowSum += s
                let idx = cast(u64) (y * (W + 1) + x)
                ii0[idx] = ii0[idx - cast(u64) (W + 1)] + rowSum
            }
        }

        // box blur via integral
        var Sb: Array'f32
        Sb.resize(cast(u64) (W * H))
        for y in 0 to (H - 1)
        {
            let y0 = Math.max(0, y - BLUR_RADIUS)
            let y1 = Math.min(H - 1, y + BLUR_RADIUS)
            for x in 0 to (W - 1)
            {
                let x0 = Math.max(0, x - BLUR_RADIUS)
                let x1 = Math.min(W - 1, x + BLUR_RADIUS)

                let A = cast(u64) (y0 * (W + 1) + x0)
                let B = cast(u64) (y0 * (W + 1) + (x1 + 1))
                let C = cast(u64) ((y1 + 1) * (W + 1) + x0)
                let D = cast(u64) ((y1 + 1) * (W + 1) + (x1 + 1))

                let sum = ii0[D] - ii0[B] - ii0[C] + ii0[A]
                let n   = cast(f32) ((x1 - x0 + 1) * (y1 - y0 + 1))
                Sb[cast(u64) (y * W + x)] = cast(f32) (sum / cast(f64) n)
            }
        }

        // integral of blurred saliency
        var ii: Array'f64
        ii.resize(cast(u64) ((W + 1) * (H + 1)))
        for y in 1 to H
        {
            var rowSum = 0.0'f64
            for x in 1 to W
            {
                let s = cast(f64) Sb[cast(u64) ((y - 1) * W + (x - 1))]
                rowSum += s
                let idx = cast(u64) (y * (W + 1) + x)
                ii[idx] = ii[idx - cast(u64) (W + 1)] + rowSum
            }
        }

        var bestX, bestY = 0's32
        var bestSum = -1.0'f64

        let maxX = W - cropW
        let maxY = H - cropH

        let cxImg           = 0.5'f64 * cast(f64) W
        let cyImg           = 0.5'f64 * cast(f64) H
        var bestCenterDist2 = 1.0e30'f64
        const EPS           = 1.0e-7'f64

        // >>> FIX: include borders (<=), not just < <<<
        for y0 in 0 to maxY
        {
            let y1 = y0 + cropH
            for x0 in 0 to maxX
            {
                let x1  = x0 + cropW
                let A   = cast(u64) (y0 * (W + 1) + x0)
                let B   = cast(u64) (y0 * (W + 1) + x1)
                let C   = cast(u64) (y1 * (W + 1) + x0)
                let D   = cast(u64) (y1 * (W + 1) + x1)
                let sum = ii[D] - ii[B] - ii[C] + ii[A]

                if sum > bestSum + EPS
                {
                    bestSum = sum
                    bestX   = x0
                    bestY   = y0
                    let cwx = cast(f64) x0 + 0.5 * cast(f64) cropW
                    let cwy = cast(f64) y0 + 0.5 * cast(f64) cropH
                    bestCenterDist2 = (cwx - cxImg) * (cwx - cxImg) + (cwy - cyImg) * (cwy - cyImg)
                }
                elif Math.abs(sum - bestSum) <= EPS
                {
                    let cwx = cast(f64) x0 + 0.5 * cast(f64) cropW
                    let cwy = cast(f64) y0 + 0.5 * cast(f64) cropH
                    let d2  = (cwx - cxImg) * (cwx - cxImg) + (cwy - cyImg) * (cwy - cyImg)
                    if d2 < bestCenterDist2
                    {
                        bestX           = x0
                        bestY           = y0
                        bestCenterDist2 = d2
                    }
                }
            }
        }

        // small refinement sweep (unit stride) around the winner
        {
            let rx0 = .clampS32(bestX - 2, 0, maxX)
            let ry0 = .clampS32(bestY - 2, 0, maxY)
            let rx1 = .clampS32(bestX + 2, 0, maxX)
            let ry1 = .clampS32(bestY + 2, 0, maxY)
            for y0 in ry0 to ry1
            {
                let y1 = y0 + cropH
                for x0 in rx0 to rx1
                {
                    let x1  = x0 + cropW
                    let A   = cast(u64) (y0 * (W + 1) + x0)
                    let B   = cast(u64) (y0 * (W + 1) + x1)
                    let C   = cast(u64) (y1 * (W + 1) + x0)
                    let D   = cast(u64) (y1 * (W + 1) + x1)
                    let sum = ii[D] - ii[B] - ii[C] + ii[A]
                    if sum > bestSum + 1e-12
                    {
                        bestSum = sum; bestX   = x0; bestY   = y0
                    }
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

    // === Smart placement + scale search ===
    private struct SearchOpts
    {
        minCoverage:      f32     // min crop area fraction (e.g. 0.55)
        blurRadius:       s32     // saliency blur radius (2–3)
        strideFrac:       f32     // scan stride as fraction of crop size
        thirdsWeight:     f32     // soft rule-of-thirds prior (placement only)
        centerWeight:     f32     // tiny center bias on near ties
    }

    private const DEFAULT_OPTS = SearchOpts{minCoverage: 0.55, blurRadius: 2, strideFrac: 0.06, thirdsWeight: 0.12, centerWeight: 0.05}

    // === Helpers: integral image, summed area, blur, thirds map ===
    private mtd const buildIntegral(src: &Array'f32, W, H: s32)->Array'f64
    {
        var ii: Array'f64
        ii.resize(cast(u64) ((W + 1) * (H + 1))) // zero-initialized
        for y in 1 to H
        {
            var rowSum = 0.0'f64
            for x in 1 to W
            {
                let s = cast(f64) src[cast(u64) ((y - 1) * W + (x - 1))]
                rowSum += s
                let idx = cast(u64) (y * (W + 1) + x)
                ii[idx] = ii[idx - cast(u64) (W + 1)] + rowSum
            }
        }
        return ii
    }

    private mtd const sumRect(ii: &Array'f64, W, H, x0, y0, x1, y1: s32)->f64
    {
        // [x0,x1) × [y0,y1)
        let A = cast(u64) (y0 * (W + 1) + x0)
        let B = cast(u64) (y0 * (W + 1) + x1)
        let C = cast(u64) (y1 * (W + 1) + x0)
        let D = cast(u64) (y1 * (W + 1) + x1)
        return ii[D] - ii[B] - ii[C] + ii[A]
    }

    private mtd const boxBlurViaIntegral(src: &Array'f32, W, H, radius: s32)->Array'f32
    {
        if radius <= 0
        {
            // return a copy to avoid aliasing surprises
            var copy: Array'f32
            copy.resize(cast(u64) (W * H))
            for i in 0 to (W * H - 1):
                copy[cast(u64) i] = src[cast(u64) i]
            return copy
        }

        var ii   = .buildIntegral(src, W, H)
        var dst: Array'f32
        dst.resize(cast(u64) (W * H))
        for y in 0 to (H - 1)
        {
            let y0 = Math.max(0, y - radius)
            let y1 = Math.min(H - 1, y + radius)
            for x in 0 to (W - 1)
            {
                let x0 = Math.max(0, x - radius)
                let x1 = Math.min(W - 1, x + radius)
                let S  = .sumRect(&ii, W, H, x0, y0, x1 + 1, y1 + 1)
                let n  = cast(f64) ((x1 - x0 + 1) * (y1 - y0 + 1))
                dst[cast(u64) (y * W + x)] = cast(f32) (S / n)
            }
        }
        return dst
    }

    private mtd const buildThirdsMap(W, H: s32, sigma: f32)->Array'f32
    {
        var tx, ty: Array'f32
        tx.resize(cast(u64) W)
        ty.resize(cast(u64) H)
        let t1:  f32 = 1.0 / 3.0
        let t2:  f32 = 2.0 / 3.0
        let sig2 = sigma * sigma
        for x in 0 to (W - 1)
        {
            let fx = cast(f32) x / cast(f32) W
            let dx = Math.min(Math.abs(fx - t1), Math.abs(fx - t2))
            tx[cast(u64) x] = Math.exp(-(dx * dx) / (2.0 * sig2))
        }
        for y in 0 to (H - 1)
        {
            let fy = cast(f32) y / cast(f32) H
            let dy = Math.min(Math.abs(fy - t1), Math.abs(fy - t2))
            ty[cast(u64) y] = Math.exp(-(dy * dy) / (2.0 * sig2))
        }
        var T: Array'f32
        T.resize(cast(u64) (W * H))
        // intersections only (composition prior lives here, not in saliency)
        for y in 0 to (H - 1):
            for x in 0 to (W - 1):
                T[cast(u64) (y * W + x)] = tx[cast(u64) x] * ty[cast(u64) y]
        return T
    }

    // === Smart search (fixed aspect, multi-scale) ===
    private mtd const calculateReframeRectSmart(targetWidth, targetHeight: f32, opts: SearchOpts = DEFAULT_OPTS)->Math.Rectangle
    {
        let W = .width
        let H = .height
        Debug.assert(W > 0 and H > 0)
        let aspect    = targetWidth / targetHeight
        let imgAspect = cast(f32) W / cast(f32) H

        // 1) Build saliency (features only), then lightly blur
        var S: Array'f32
        S.resize(cast(u64) (W * H))
        .computeSaliencyMap(&S, DEFAULT_WEIGHTS)

        var Sb = .boxBlurViaIntegral(&S, W, H, opts.blurRadius)

        // 2) Composition prior (thirds intersections) — unified sigma
        var T   = .buildThirdsMap(W, H, THIRDS_SIGMA)
        var iiS = .buildIntegral(&Sb, W, H)
        var iiT = .buildIntegral(&T, W, H)

        let cx = 0.5'f64 * cast(f64) W
        let cy = 0.5'f64 * cast(f64) H

        // 3) Multi-scale search
        let imgArea = cast(f64) (W * H)
        let minArea = cast(f64) opts.minCoverage * imgArea

        var bestX, bestY, bestW, bestH = 0's32
        var bestScore    = -1.0e30'f64
        var bestCenterD2 = 1.0e30'f64

        // pre-scan to estimate dynamic range of mean-saliency to scale center bias on near-ties
        var meanMin = 1.0e30'f64
        var meanMax = -1.0e30'f64

        const STEPS = 12's32
        for step in 0 to (STEPS - 1)
        {
            let frac       = 1.0 - (cast(f64) step / cast(f64) (STEPS - 1)) * (1.0 - cast(f64) opts.minCoverage)
            let targetArea = Math.max(frac * imgArea, minArea)

            var cw, ch: s32
            if imgAspect > aspect
            {
                ch = Math.min(H, Math.max(8, cast(s32) Math.round(Math.sqrt(targetArea / cast(f64) aspect))))
                cw = Math.min(W, cast(s32) Math.round(cast(f64) ch * cast(f64) aspect))
            }
            else
            {
                cw = Math.min(W, Math.max(8, cast(s32) Math.round(Math.sqrt(targetArea * cast(f64) aspect))))
                ch = Math.min(H, cast(s32) Math.round(cast(f64) cw / cast(f64) aspect))
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
                let y1 = y0 + ch
                for var x0 = 0; x0 <= maxX; x0 += strideX
                {
                    let x1 = x0 + cw

                    let sumS = .sumRect(&iiS, W, H, x0, y0, x1, y1)
                    let sumT = .sumRect(&iiT, W, H, x0, y0, x1, y1)
                    let area = cast(f64) (cw * ch)

                    let meanS = (sumS / area)
                    meanMin = Math.min(meanMin, meanS)
                    meanMax = Math.max(meanMax, meanS)

                    var score = meanS + cast(f64) opts.thirdsWeight * (sumT / area)

                    let cwx = cast(f64) x0 + 0.5 * cw
                    let cwy = cast(f64) y0 + 0.5 * ch
                    let d2  = (cwx - cx) * (cwx - cx) + (cwy - cy) * (cwy - cy)

                    // center only breaks near ties later; don't subtract yet
                    if score > bestScore
                    {
                        bestScore    = score; bestX        = x0; bestY        = y0; bestW        = cw; bestH        = ch; bestCenterD2 = d2; }
                    elif Math.abs(score - bestScore) <= 1e-12
                    {
                        // near-tie → prefer closer to center
                        if d2 < bestCenterD2
                        {
                            bestX        = x0; bestY        = y0; bestW        = cw; bestH        = ch; bestCenterD2 = d2; }
                    }
                }
            }
        }

        // 3b) Apply a tiny center bias only if very close in score (scale by observed range)
        // (already handled during scan via tie-break; optional: no-op here)

        // 4) Fallback
        if bestW == 0 or bestH == 0:
            return .calculateReframeRect(targetWidth, targetHeight)

        // 5) Local refinement (position, stride 1) around best window; also one-step scale tweak
        {
            let maxX = W - bestW
            let maxY = H - bestH

            var improved = true
            var iter     = 0
            while improved and iter < 3
            {
                improved = false; iter += 1
                let rx0 = .clampS32(bestX - 2, 0, maxX)
                let ry0 = .clampS32(bestY - 2, 0, maxY)
                let rx1 = .clampS32(bestX + 2, 0, maxX)
                let ry1 = .clampS32(bestY + 2, 0, maxY)
                for y0 in ry0 to ry1
                {
                    let y1 = y0 + bestH
                    for x0 in rx0 to rx1
                    {
                        let x1    = x0 + bestW
                        let sumS  = .sumRect(&iiS, W, H, x0, y0, x1, y1)
                        let sumT  = .sumRect(&iiT, W, H, x0, y0, x1, y1)
                        let area  = cast(f64) (bestW * bestH)
                        var score = (sumS / area) + cast(f64) opts.thirdsWeight * (sumT / area)
                        if score > bestScore + 1e-12
                        {
                            bestScore = score; bestX     = x0; bestY     = y0; improved  = true; }
                    }
                }
            }

            // quick +/- one scale rung test at the same aspect
            {
                let bigger = 1.06'f64, smaller = 1.0 / 1.06'f64
                for scaleTest in 0 to 1
                {
                    let sf = (scaleTest == 0) ? bigger : smaller
                    var cw = cast(s32) Math.round(cast(f64) bestW * sf)
                    var ch = cast(s32) Math.round(cast(f64) bestH * sf)
                    if cw < 4 or ch < 4:
                        continue
                    if cw > W or ch > H:
                        continue
                    // keep aspect exact
                    if imgAspect > aspect:
                        cw = cast(s32) Math.round(cast(f64) ch * cast(f64) aspect)
                    else:
                        ch = cast(s32) Math.round(cast(f64) cw / cast(f64) aspect)

                    let maxX2 = W - cw
                    let maxY2 = H - ch
                    let x0    = .clampS32(bestX + (bestW - cw) / 2, 0, maxX2)
                    let y0    = .clampS32(bestY + (bestH - ch) / 2, 0, maxY2)
                    let x1    = x0 + cw
                    let y1    = y0 + ch

                    let sumS  = .sumRect(&iiS, W, H, x0, y0, x1, y1)
                    let sumT  = .sumRect(&iiT, W, H, x0, y0, x1, y1)
                    let area  = cast(f64) (cw * ch)
                    let score = (sumS / area) + cast(f64) opts.thirdsWeight * (sumT / area)
                    if score > bestScore + 1e-12
                    {
                        bestScore = score; bestX     = x0; bestY     = y0; bestW     = cw; bestH     = ch; }
                }
            }
        }

        var r: retval
        r.x      = bestX; r.y      = bestY; r.width  = bestW; r.height = bestH
        return r
    }

    private mtd const findInterestPoint()->InterestPoint
    {
        var saliencyMap: Array'f32
        saliencyMap.resize(cast(u64) (.width * .height))
        .computeSaliencyMap(&saliencyMap, DEFAULT_WEIGHTS)

        var maxScore = -1.0'f32
        var maxX, maxY = 0's32

        for y in 0 to (.height - 1)
        {
            for x in 0 to (.width - 1)
            {
                let idx   = cast(u64) y * cast(u64) .width + cast(u64) x
                let score = saliencyMap[idx]
                if score > maxScore
                {
                    maxScore = score; maxX     = x; maxY     = y; }
            }
        }

        var out: retval
        out.x     = cast(f32) maxX / cast(f32) .width
        out.y     = cast(f32) maxY / cast(f32) .height
        out.score = maxScore
        return out
    }

    // Robust percentile estimation (for normalization) via sampling
    private mtd const estimatePercentiles(buf: &Array'f32, N: s32, pLow, pHigh: f32)->{ a: f32, b: f32 }
    {
        // reservoir-sample up to 4096 values evenly
        const K  = 4096's32
        var tmp: Array'f32
        var M    = Math.min(K, N)
        tmp.resize(cast(u64) M)

        if N <= K
        {
            for i in 0 to (N - 1):
                tmp[cast(u64) i] = buf[cast(u64) i]
        }
        else
        {
            // stride sampling
            let step = cast(s32) Math.floor(cast(f32) N / cast(f32) M)
            var w    = 0's32
            for var i = 0; i < N; i += step
            {
                if w >= M:
                    break
                tmp[cast(u64) w] = buf[cast(u64) i]; w += 1
            }
            M = w
        }

        tmp.sortReverse()

        let il = Math.max(0, Math.min(M - 1, cast(s32) Math.round(pLow * cast(f32) (M - 1))))
        let ih = Math.max(0, Math.min(M - 1, cast(s32) Math.round(pHigh * cast(f32) (M - 1))))
        let lo = tmp[cast(u64) il]
        let hi = tmp[cast(u64) ih]
        if hi <= lo:
            return {lo, lo + 1e-6}
        return {lo, hi}
    }

    // === Core saliency (contrast, saturation, edge). Thirds removed here. ===
    private mtd const computeSaliencyMap(saliencyMap: &Array'f32, weights: SaliencyWeights)
    {
        let W  = .width
        let H  = .height
        let N  = cast(s32) (W * H)
        let Nu = cast(u64) N

        var lum, sat: Array'f32
        lum.resize(cast(u64) N)
        sat.resize(cast(u64) N)

        // one pass: cache linear luminance + saturation (HSV-ish, ε-stable)
        var f   = {lum: &lum, sat: &sat}
        let mec = cast #unconst (*Image) me
        mec.visitPixels(&f)
        {
            let p = image.getPixelColor(x, y)
            // assume p.r/g/b in [0,1] sRGB. Convert to linear
            let r = image.srgbToLinear(cast(f32) p.r)
            let g = image.srgbToLinear(cast(f32) p.g)
            let b = image.srgbToLinear(cast(f32) p.b)

            let params = cast(*#decltype(f)) userData

            // luminance (BT.709-ish)
            let L = r * 0.2126 + g * 0.7152 + b * 0.0722
            params.lum.buffer[index] = L

            // saturation (HSV-like), ε-stable
            let maxC = Math.max(Math.max(r, g), b)
            let minC = Math.min(Math.min(r, g), b)
            let eps  = 1e-6'f32
            params.sat.buffer[index] = (maxC > 0.0) ? ((maxC - minC) / (maxC + eps)) : 0.0
        }

        // Integral images for local variance (contrast)
        var ii, iiSq: Array'f64
        ii.resize(cast(u64) ((W + 1) * (H + 1)))
        iiSq.resize(cast(u64) ((W + 1) * (H + 1)))
        for y in 1 to H
        {
            var rowSum   = 0.0'f64
            var rowSumSq = 0.0'f64
            for x in 1 to W
            {
                let Ld = cast(f64) lum[cast(u64) ((y - 1) * W + (x - 1))]
                rowSum += Ld
                rowSumSq += Ld * Ld
                let idxI = cast(u64) (y * (W + 1) + x)
                ii[idxI]   = ii[idxI - cast(u64) (W + 1)] + rowSum
                iiSq[idxI] = iiSq[idxI - cast(u64) (W + 1)] + rowSumSq
            }
        }

        // Pre-blur luminance (radius 1) before Sobel for cleaner edges
        var lumBlur = .boxBlurViaIntegral(&lum, W, H, 1)

        // Sobel edges from blurred luminance
        var edge: Array'f32
        edge.resize(cast(u64) N)
        for y in 0 until H
        {
            for x in 0 until W
            {
                let i = cast(u64) (y * W + x)
                if x == 0 or y == 0 or x == W - 1 or y == H - 1
                {
                    edge[i] = 0.0; continue
                }

                // neighbors from lumBlur
                let Lm1m1 = lumBlur[cast(u64) ((y - 1) * W + (x - 1))]
                let Lp1m1 = lumBlur[cast(u64) ((y - 1) * W + (x + 1))]
                let Lm10  = lumBlur[cast(u64) (y * W + (x - 1))]
                let Lp10  = lumBlur[cast(u64) (y * W + (x + 1))]
                let Lm1p1 = lumBlur[cast(u64) ((y + 1) * W + (x - 1))]
                let Lp1p1 = lumBlur[cast(u64) ((y + 1) * W + (x + 1))]
                let L0m1  = lumBlur[cast(u64) ((y - 1) * W + x)]
                let L0p1  = lumBlur[cast(u64) ((y + 1) * W + x)]

                let gx = -1.0 * Lm1m1 + 1.0 * Lp1m1 + -2.0 * Lm10 + 2.0 * Lp10 + -1.0 * Lm1p1 + 1.0 * Lp1p1
                let gy = -1.0 * Lm1m1 - 2.0 * L0m1 - 1.0 * Lp1m1 + 1.0 * Lm1p1 + 2.0 * L0p1 + 1.0 * Lp1p1

                edge[i] = cast(f32) Math.sqrt(gx * gx + gy * gy)
            }
        }

        // Local contrast via integral variance
        const R       = SALIENCY_RADIUS
        var contrast: Array'f32
        contrast.resize(cast(u64) N)

        for y in 0 until H
        {
            for x in 0 until W
            {
                let i  = cast(u64) (y * W + x)
                let x0 = Math.max(0, x - R)
                let y0 = Math.max(0, y - R)
                let x1 = Math.min(W - 1, x + R)
                let y1 = Math.min(H - 1, y + R)

                let A = cast(u64) (y0 * (W + 1) + x0)
                let B = cast(u64) (y0 * (W + 1) + (x1 + 1))
                let C = cast(u64) ((y1 + 1) * (W + 1) + x0)
                let D = cast(u64) ((y1 + 1) * (W + 1) + (x1 + 1))

                let n     = cast(f64) ((x1 - x0 + 1) * (y1 - y0 + 1))
                let sum   = ii[D] - ii[B] - ii[C] + ii[A]
                let sumSq = iiSq[D] - iiSq[B] - iiSq[C] + iiSq[A]

                let mean = sum / n
                let ovar = Math.max(0.0, (sumSq / n) - mean * mean)
                contrast[i] = cast(f32) Math.sqrt(ovar)
            }
        }

        // Robust per-feature normalization via sampled percentiles (2–98%)
        let (cLo, cHi) = .estimatePercentiles(&contrast, N, 0.02, 0.98)
        let (sLo, sHi) = .estimatePercentiles(&sat, N, 0.02, 0.98)
        let (eLo, eHi) = .estimatePercentiles(&edge, N, 0.02, 0.98)
        let cDen = cHi - cLo
        let sDen = sHi - sLo
        let eDen = eHi - eLo

        for i in 0 to (N - 1)
        {
            let cn = .clamp01f((contrast[cast(u64) i] - cLo) / cDen)
            let sn = .clamp01f((sat[cast(u64) i] - sLo) / sDen)
            let en = .clamp01f((edge[cast(u64) i] - eLo) / eDen)
            saliencyMap[cast(u64) i] = weights.contrast * cn + weights.saturation * sn + weights.edge * en
        }
    }
}

public impl Image
{
    // Crop only (no upscaling)
    mtd smartCrop(rect: Math.Rectangle)
    {
        Debug.assert(rect.width > 0 and rect.height > 0)
        .crop(.calculateReframeRect(rect.width, rect.height))
    }

    // Crop with upscaling, using smarter multi-scale search
    mtd smartCropScale(rect: Math.Rectangle)
    {
        Debug.assert(rect.width > 0 and rect.height > 0)
        .crop(.calculateReframeRectSmart(rect.width, rect.height))
    }
}
