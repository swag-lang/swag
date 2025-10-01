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

    // === Tunables ===
    private const SALIENCY_RADIUS = 5
    private const THIRDS_SIGMA    = 0.08
    private const DEFAULT_WEIGHTS = SaliencyWeights{contrast: 1.0, thirds: 0.8, saturation: 0.6, edge: 0.8}

    // Compute the largest in-bounds rectangle with target aspect ratio, positioned by saliency.
    // This only TRANSLATES the window and possibly CROPS to match aspect; it never requests scaling.
    private mtd const calculateReframeRect(targetWidth, targetHeight: f32)->Math.Rectangle
    {
        // 1) Desired aspect
        let aspect    = targetWidth / targetHeight
        let W         = .width
        let H         = .height
        let imgAspect = cast(f32) W / cast(f32) H

        // 2) Maximal crop that matches aspect and fits inside the image
        var cropW: s32
        var cropH: s32
        if imgAspect > aspect
        {
            // Image is wider than target aspect: limit width
            cropH = H
            cropW = cast(s32) Math.floor(aspect * cast(f32) cropH + 0.5)
        }
        else
        {
            // Image is taller than target aspect: limit height
            cropW = W
            cropH = cast(s32) Math.floor(cast(f32) cropW / aspect + 0.5)
        }

        // 3) Place the crop so that the interest point is near its center (clamped to bounds)
        let ip = .findInterestPoint()
        let cx = cast(s32) (Math.saturate(ip.x) * cast(f32) W)
        let cy = cast(s32) (Math.saturate(ip.y) * cast(f32) H)

        var x = cx - cropW / 2
        var y = cy - cropH / 2

        if x < 0:
            x = 0
        if y < 0:
            y = 0
        if x + cropW > W:
            x = W - cropW
        if y + cropH > H:
            y = H - cropH

        var r: retval
        r.x      = x; r.y      = y; r.width  = cropW; r.height = cropH
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

        for y in H
        {
            for x in W
            {
                let idx = cast(u64) y * cast(u64) W + cast(u64) x
                let p   = .getPixelColor(x, y)                        // SINGLE call per pixel

                let rf = cast(f32) p.r
                let gf = cast(f32) p.g
                let bf = cast(f32) p.b

                // Luminance cache
                let L = rf * 0.299 + gf * 0.587 + bf * 0.114
                lum[idx] = L

                // Saturation cache (HSV-like)
                let maxC = Math.max(Math.max(rf, gf), bf)
                let minC = Math.min(Math.min(rf, gf), bf)
                sat[idx] = (maxC > 0.0) ? ((maxC - minC) / maxC) : 0.0
            }
        }

        // Integral images over luminance for O(1) local variance (contrast)
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

        // Precompute rule-of-thirds weights (separable)
        var thirdsX, thirdsY: Array'f32
        thirdsX.resize(cast(u64) W)
        thirdsY.resize(cast(u64) H)
        {
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
                let i = cast(u64) (y * W + x)

                // Box bounds
                let x0 = Math.max(0's32, x - R)
                let y0 = Math.max(0's32, y - R)
                let x1 = Math.min(W - 1, x + R)
                let y1 = Math.min(H - 1, y + R)

                // Integral indices (with +1 padding)
                let A = cast(u64) (y0 * (W + 1) + x0)
                let B = cast(u64) (y0 * (W + 1) + (x1 + 1))
                let C = cast(u64) ((y1 + 1) * (W + 1) + x0)
                let D = cast(u64) ((y1 + 1) * (W + 1) + (x1 + 1))

                let n     = cast(f32) ((x1 - x0 + 1) * (y1 - y0 + 1))
                let sum   = cast(f32) (ii[D] - ii[B] - ii[C] + ii[A])
                let sumSq = cast(f32) (iiSq[D] - iiSq[B] - iiSq[C] + iiSq[A])

                let mean = sum / n
                let ovar = Math.max(0.0, (sumSq / n) - mean * mean)
                let c    = Math.sqrt(ovar)

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
                let i = cast(u64) (y * W + x)
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