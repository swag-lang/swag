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
        thirds:         f32
        saturation:     f32
        edge:           f32
    }

    // Tunables
    private const SALIENCY_RADIUS = 5
    private const THIRDS_SIGMA = 0.08
    private const DEFAULT_WEIGHTS = SaliencyWeights{contrast: 1.0, thirds: 0.8, saturation: 0.6, edge: 0.8}

    // Compute the largest in-bounds rectangle with target aspect ratio, positioned by saliency.
    // This only TRANSLATES the window and possibly CROPS to match aspect; it never requests scaling.
    // Compute the largest in-bounds rectangle with target aspect ratio, positioned by saliency sum.
    // Compute the largest in-bounds rectangle with target aspect ratio, positioned by the *sum of
    // (lightly blurred) saliency*.
    // Tie-break equal scores by preferring crops nearer the image center.
    private mtd const calculateReframeRect(targetWidth, targetHeight: f32)->Math.Rectangle
    {
        // --- aspect & maximal in-bounds crop ---
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

        // build saliency map as-is
        var S: Array'f32
        S.resize(cast(u64) (W * H))
        .computeSaliencyMap(&S, DEFAULT_WEIGHTS)

        // quick de-spike: tiny box blur of S using an integral image
        const BLUR_RADIUS: s32 = 2 // very small, keeps structure, kills single-pixel spikes

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

        // produce blurred S into Sb (same size as S)
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

                // +1 padding for integral coords
                let A = cast(u64) (y0 * (W + 1) + x0)
                let B = cast(u64) (y0 * (W + 1) + (x1 + 1))
                let C = cast(u64) ((y1 + 1) * (W + 1) + x0)
                let D = cast(u64) ((y1 + 1) * (W + 1) + (x1 + 1))

                let sum = ii0[D] - ii0[B] - ii0[C] + ii0[A]
                let n   = cast(f32) ((x1 - x0 + 1) * (y1 - y0 + 1))
                Sb[cast(u64) (y * W + x)] = cast(f32) (sum / cast(f64) n)
            }
        }

        // integral of blurred saliency (this is what we optimize over)
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

        // argmax: summed blurred saliency in the crop window
        var bestX, bestY = 0's32
        var bestSum = -1.0'f64

        let maxX = W - cropW
        let maxY = H - cropH

        // center tie-breaker
        let cxImg           = 0.5'f64 * cast(f64) W
        let cyImg           = 0.5'f64 * cast(f64) H
        var bestCenterDist2 = 1.0e30'f64
        const EPS           = 1.0e-7'f64

        for y0 in 0 to maxY
        {
            let y1 = y0 + cropH
            for x0 in 0 to maxX
            {
                let x1 = x0 + cropW

                // integral coords (+1 offset)
                let A = cast(u64) (y0 * (W + 1) + x0)
                let B = cast(u64) (y0 * (W + 1) + x1)
                let C = cast(u64) (y1 * (W + 1) + x0)
                let D = cast(u64) (y1 * (W + 1) + x1)

                let sum = ii[D] - ii[B] - ii[C] + ii[A]

                if sum > bestSum + EPS
                {
                    bestSum = sum
                    bestX   = x0
                    bestY   = y0
                    // update tiebreak values
                    let cwx = cast(f64) x0 + 0.5 * cast(f64) cropW
                    let cwy = cast(f64) y0 + 0.5 * cast(f64) cropH
                    bestCenterDist2 = (cwx - cxImg) * (cwx - cxImg) + (cwy - cyImg) * (cwy - cyImg)
                }
                elif Math.abs(sum - bestSum) <= EPS
                {
                    // tie-break: prefer nearer to image center
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

        var r: retval
        r.x      = bestX
        r.y      = bestY
        r.width  = cropW
        r.height = cropH
        return r
    }

    // === New tunables/options for smarter placement + scale search ===
    private struct SearchOpts
    {
        minCoverage:      f32     // min crop area fraction relative to image (e.g., 0.55 means allow up to ~1.35x zoom-in)
        blurRadius:       s32     // box-blur radius on saliency (2–3 is good)
        strideFrac:       f32     // sliding-window stride as a fraction of crop size (0.04–0.08)
        thirdsWeight:     f32     // small positive to softly prefer rule of thirds
        centerWeight:     f32     // tiny positive to softly prefer center when scores tie
    }

    private const DEFAULT_OPTS = SearchOpts{minCoverage: 0.55, // allow zoom-in until the crop covers at least 55% of the image area
    blurRadius: 2, strideFrac: 0.06, thirdsWeight: 0.12, centerWeight: 0.05}

    // === Helpers: integral image, summed area, blur, thirds map ===
    private mtd const buildIntegral(src: &Array'f32, W, H: s32)->Array'f64
    {
        var ii: Array'f64
        ii.resize(cast(u64) ((W + 1) * (H + 1))) // zero-initialized padding
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
        // inclusive-exclusive: x in [x0, x1), y in [y0, y1)
        // integral coords already +1 padded
        let A = cast(u64) (y0 * (W + 1) + x0)
        let B = cast(u64) (y0 * (W + 1) + x1)
        let C = cast(u64) (y1 * (W + 1) + x0)
        let D = cast(u64) (y1 * (W + 1) + x1)
        return ii[D] - ii[B] - ii[C] + ii[A]
    }

    private mtd const boxBlurViaIntegral(src: &Array'f32, W, H, radius: s32)->Array'f32
    {
        if radius <= 0:
            return src
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
                // convert to integral’s [..)+1 coordinates
                let S = .sumRect(&ii, W, H, x0, y0, x1 + 1, y1 + 1)
                let n = cast(f64) ((x1 - x0 + 1) * (y1 - y0 + 1))
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
        let t1:   f32 = 1.0 / 3.0
        let t2:   f32 = 2.0 / 3.0
        let sig2: f32 = sigma * sigma
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
        for y in 0 until H:
            for x in 0 until W:
                T[cast(u64) (y * W + x)] = tx[cast(u64) x] * ty[cast(u64) y] // intersections glow
        return T
    }

    // === New: multi-scale, integral-optimized smart crop ===
    private mtd const calculateReframeRectSmart(targetWidth, targetHeight: f32, opts: SearchOpts = DEFAULT_OPTS)->Math.Rectangle
    {
        let W = .width
        let H = .height
        Debug.assert(W > 0 and H > 0)
        let aspect    = targetWidth / targetHeight
        let imgAspect = cast(f32) W / cast(f32) H

        // 1) Build saliency once, then lightly blur for stability
        var S: Array'f32
        S.resize(cast(u64) (W * H))
        .computeSaliencyMap(&S, DEFAULT_WEIGHTS)

        var Sb = .boxBlurViaIntegral(&S, W, H, opts.blurRadius)

        // 2) Optional composition prior (very soft)
        var T   = .buildThirdsMap(W, H, 0.08)     // sigma can be tuned
        var iiS = .buildIntegral(&Sb, W, H)
        var iiT = .buildIntegral(&T, W, H)

        // center (for tie-break)
        let cx = 0.5'f64 * cast(f64) W
        let cy = 0.5'f64 * cast(f64) H

        // 3) Multi-scale search over crop sizes at fixed aspect
        //    We scan from the largest crop down to minCoverage of image area.
        let imgArea = cast(f64) (W * H)
        let minArea = cast(f64) opts.minCoverage * imgArea

        var bestX, bestY, bestW, bestH = 0's32
        var bestScore    = -1.0e30'f64
        var bestCenterD2 = 1.0e30'f64

        // coverage ladder: ~12 steps is plenty; you can increase for more precision
        const STEPS = 12's32
        for step in 0 to (STEPS - 1)
        {
            // linear in area fraction
            let frac       = 1.0 - (cast(f64) step / cast(f64) (STEPS - 1)) * (1.0 - cast(f64) opts.minCoverage)
            let targetArea = Math.max(frac * imgArea, minArea)

            // derive crop size at fixed aspect
            var cw, ch: s32
            if imgAspect > aspect
            {
                // height-limited path for largest feasible height at this area
                ch = Math.min(H, Math.max(8, cast(s32) Math.round(Math.sqrt(targetArea / cast(f64) aspect))))
                cw = Math.min(W, cast(s32) Math.round(cast(f64) ch * cast(f64) aspect))
            }
            else
            {
                cw = Math.min(W, Math.max(8, cast(s32) Math.round(Math.sqrt(targetArea * cast(f64) aspect))))
                ch = Math.min(H, cast(s32) Math.round(cast(f64) cw / cast(f64) aspect))
            }

            // skip degenerate
            if cw < 4 or ch < 4:
                continue

            let maxX = W - cw
            let maxY = H - ch
            if maxX < 0 or maxY < 0:
                continue

            // stride proportional to crop size for speed vs. quality tradeoff
            let strideX = Math.max(1, cast(s32) Math.round(opts.strideFrac * cast(f32) cw))
            let strideY = Math.max(1, cast(s32) Math.round(opts.strideFrac * cast(f32) ch))

            for var y0 = 0; y0 < maxY; y0 += strideY
            {
                let y1 = y0 + ch
                for var x0 = 0; x0 < maxX; x0 += strideX
                {
                    let x1 = x0 + cw

                    // sums via integrals (inclusive-exclusive with +1)
                    let sumS = .sumRect(&iiS, W, H, x0, y0, x1, y1)
                    let sumT = .sumRect(&iiT, W, H, x0, y0, x1, y1)
                    let area = cast(f64) (cw * ch)

                    // objective: maximize mean blurred saliency + small thirds bias
                    var score = (sumS / area) + cast(f64) opts.thirdsWeight * (sumT / area)

                    // gentle center tie-break when scores are near
                    let cwx = cast(f64) x0 + 0.5 * cw
                    let cwy = cast(f64) y0 + 0.5 * ch
                    let d2  = (cwx - cx) * (cwx - cx) + (cwy - cy) * (cwy - cy)
                    // subtract tiny center penalty so nearer center is larger score on ties
                    score -= cast(f64) opts.centerWeight * (d2 / (cast(f64) W * W + cast(f64) H * H))

                    if score > bestScore
                    {
                        bestScore    = score
                        bestX        = x0; bestY        = y0; bestW        = cw; bestH        = ch
                        bestCenterD2 = d2
                    }
                    elif Math.abs(score - bestScore) <= 1e-9
                    {
                        // exact tie → prefer closer to center
                        if d2 < bestCenterD2
                        {
                            bestX        = x0; bestY        = y0; bestW        = cw; bestH        = ch
                            bestCenterD2 = d2
                        }
                    }
                }
            }
        }

        // 4) Fallback (shouldn't happen, but be safe)
        if bestW == 0 or bestH == 0
        {
            // revert to your largest in-bounds crop centered on max saliency
            return .calculateReframeRect(targetWidth, targetHeight)
        }

        // 5) Return the best crop
        var r: retval
        r.x      = bestX; r.y      = bestY; r.width  = bestW; r.height = bestH
        return r
    }

    private mtd const findInterestPoint()->InterestPoint
    {
        var saliencyMap: Array'f32
        saliencyMap.resize(cast(u64) (.width * .height))
        .computeSaliencyMap(&saliencyMap, DEFAULT_WEIGHTS)

        var maxScore = -1.0
        var maxX, maxY = 0's32

        for y in .height
        {
            for x in .width
            {
                let idx   = cast(u64) y * cast(u64) .width + cast(u64) x
                let score = saliencyMap[idx]
                if score > maxScore
                {
                    maxScore = score
                    maxX     = x
                    maxY     = y
                }
            }
        }

        var out: retval
        out.x     = cast(f32) maxX / cast(f32) .width
        out.y     = cast(f32) maxY / cast(f32) .height
        out.score = maxScore
        return out
    }

    private mtd const computeSaliencyMap(saliencyMap: &Array'f32, weights: SaliencyWeights)
    {
        let W = .width
        let H = .height
        let N = cast(u64) (W * H)

        // Per-pixel caches (exactly 1 getPixelColor per pixel)
        var lum, sat: Array'f32
        lum.resize(N) // luminance
        sat.resize(N) // saturation

        var f   = {lum: &lum, sat: &sat}
        let mec = cast #unconst (*Image) me
        mec.visitPixels(&f)
        {
            let p = image.getPixelColor(x, y)

            let rf = cast(f32) p.r
            let gf = cast(f32) p.g
            let bf = cast(f32) p.b

            let params = cast(*#decltype(f)) userData

            // Luminance cache
            let L = rf * 0.299 + gf * 0.587 + bf * 0.114
            params.lum.buffer[index] = L

            // Saturation cache (HSV-like)
            let maxC = Math.max(Math.max(rf, gf), bf)
            let minC = Math.min(Math.min(rf, gf), bf)
            params.sat.buffer[index] = (maxC > 0.0) ? ((maxC - minC) / maxC) : 0.0
        }

        // Integral images over luminance for O(1) local variance (contrast)
        var ii, iiSq: Array'f64
        ii.resize((W + 1) * (H + 1))
        iiSq.resize((W + 1) * (H + 1))

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

        // Precompute rule-of-thirds weights (separable)
        var thirdsX, thirdsY: Array'f32
        thirdsX.resize(W)
        thirdsY.resize(H)

        let t1 = 1.0 / 3.0
        let t2 = 2.0 / 3.0
        for x in W
        {
            let fx = cast(f32) x / cast(f32) W
            let dx = Math.min(Math.abs(fx - t1), Math.abs(fx - t2))
            thirdsX[cast(u64) x] = Math.exp(-(dx * dx) / (2.0 * THIRDS_SIGMA * THIRDS_SIGMA))
        }

        for y in H
        {
            let fy = cast(f32) y / cast(f32) H
            let dy = Math.min(Math.abs(fy - t1), Math.abs(fy - t2))
            thirdsY[cast(u64) y] = Math.exp(-(dy * dy) / (2.0 * THIRDS_SIGMA * THIRDS_SIGMA))
        }

        // Sobel edges from cached luminance
        var edge: Array'f32
        edge.resize(N)
        for y in H
        {
            for x in W
            {
                let i = cast(u64) (y * W + x)
                if x == 0 or y == 0 or x == W - 1 or y == H - 1
                {
                    edge[i] = 0.0
                    continue
                }

                // neighbors from lum
                let Lm1m1 = lum[((y - 1) * W + (x - 1))]
                let Lp1m1 = lum[((y - 1) * W + (x + 1))]
                let Lm10  = lum[(y * W + (x - 1))]
                let Lp10  = lum[(y * W + (x + 1))]
                let Lm1p1 = lum[((y + 1) * W + (x - 1))]
                let Lp1p1 = lum[((y + 1) * W + (x + 1))]
                let L0m1  = lum[((y - 1) * W + x)]
                let L0p1  = lum[((y + 1) * W + x)]

                let gx = -1.0 * Lm1m1 + 1.0 * Lp1m1 + -2.0 * Lm10 + 2.0 * Lp10 + -1.0 * Lm1p1 + 1.0 * Lp1p1
                let gy = -1.0 * Lm1m1 - 2.0 * L0m1 - 1.0 * Lp1m1 + 1.0 * Lm1p1 + 2.0 * L0p1 + 1.0 * Lp1p1

                edge[i] = Math.sqrt(gx * gx + gy * gy) // range normalized later
            }
        }

        // Feature ranges (for normalization)
        var cMin, tMin, sMin, eMin = Swag.F32.Inf
        var cMax, tMax, sMax, eMax = Swag.F32.NegInf

        var contrast, thirds: Array'f32
        contrast.resize(N)
        thirds.resize(N)

        const R = SALIENCY_RADIUS

        // First pass: compute contrast & thirds, and track mins/maxes (saturation/edge already cached)
        for y in H
        {
            for x in W
            {
                let i = y * W + x

                // Box bounds
                let x0 = Math.max(0, x - R)
                let y0 = Math.max(0, y - R)
                let x1 = Math.min(W - 1, x + R)
                let y1 = Math.min(H - 1, y + R)

                // Integral indices (with +1 padding)
                let A = y0 * (W + 1) + x0
                let B = y0 * (W + 1) + (x1 + 1)
                let C = (y1 + 1) * (W + 1) + x0
                let D = (y1 + 1) * (W + 1) + (x1 + 1)

                let n     = (x1 - x0 + 1) * (y1 - y0 + 1)
                let sum   = ii[D] - ii[B] - ii[C] + ii[A]
                let sumSq = iiSq[D] - iiSq[B] - iiSq[C] + iiSq[A]

                let mean = sum / n
                let ovar = Math.max(0.0, (sumSq / n) - mean * mean)
                let c    = cast(f32) Math.sqrt(ovar)

                let t = thirdsX[x] + thirdsY[y]

                contrast[i] = c
                thirds[i]   = t

                cMin = Math.min(cMin, c); cMax = Math.max(cMax, c)
                tMin = Math.min(tMin, t); tMax = Math.max(tMax, t)

                let s = sat[i]
                sMin = Math.min(sMin, s); sMax = Math.max(sMax, s)

                let e = edge[i]
                eMin = Math.min(eMin, e); eMax = Math.max(eMax, e)
            }
        }

        let cDen = (cMax > cMin) ? (cMax - cMin) : 1.0
        let tDen = (tMax > tMin) ? (tMax - tMin) : 1.0
        let sDen = (sMax > sMin) ? (sMax - sMin) : 1.0
        let eDen = (eMax > eMin) ? (eMax - eMin) : 1.0

        // Second pass: normalize and combine
        for y in H
        {
            for x in W
            {
                let i  = cast(u64) (y * W + x)
                let cn = (contrast[i] - cMin) / cDen
                let tn = (thirds[i] - tMin) / tDen
                let sn = (sat[i] - sMin) / sDen
                let en = (edge[i] - eMin) / eDen
                saliencyMap[i] = weights.contrast * cn + weights.thirds * tn + weights.saturation * sn + weights.edge * en
            }
        }
    }
}

public impl Image
{
    // Reframe to target aspect by cropping only (no resize / no zoom)
    // Returns a new image cropped to the largest rectangle of the requested aspect,
    // positioned so the interest point is near the center, but NEVER scaled up.
    mtd smartCrop(rect: Math.Rectangle)
    {
        Debug.assert(rect.width > 0 and rect.height > 0)
        .crop(.calculateReframeRectSmart(rect.width, rect.height))
    }
}
