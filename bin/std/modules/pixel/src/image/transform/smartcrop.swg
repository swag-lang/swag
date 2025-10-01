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
        .crop(.calculateReframeRect(rect.width, rect.height))
    }
}
