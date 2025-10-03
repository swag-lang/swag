using Core

public impl Image
{
    private struct InterestPoint
    {
        x, y:      f32  // Normalized coordinates (0.0 to 1.0) of the most salient point
        score:     f32  // Saliency score at this point
    }

    // Weights for combining different saliency factors
    private struct SaliencyWeights
    {
        contrast:       f32  // Weight for local luminance variation (typically 1.0)
        saturation:     f32  // Weight for color intensity (typically 0.55)
        edge:           f32  // Weight for edge strength (typically 1.2)
    }

    // Context passed to scoring functions to avoid repeated parameter passing
    private struct ScoreCtx
    {
        iiS:         *Array'f64     // Integral image of saliency values
        iiT:         *Array'f64     // Integral image of rule-of-thirds weights
        iiE:         *Array'f64     // Integral image of edge distance values
        w, h:        s32            // Image dimensions
        imgArea:     f64            // Total image area (w * h)
        thirdsW:     f64            // Weight for rule of thirds score
        centerW:     f64            // Weight for center proximity score
        shrinkW:     f64            // Weight favoring larger crops
        edgePen:     f64            // Penalty for cutting through edges
        cx, cy:      f64            // Image center coordinates
        d2Max:       f64            // Maximum squared distance from center (for normalization)
    }

    // Radius for local contrast calculation (6 pixels in each direction)
    private const SALIENCY_RADIUS = 6
    // Sigma for Gaussian falloff in rule-of-thirds map (7.5% of image dimension)
    private const THIRDS_SIGMA = 0.075
    // Small epsilon for tie-breaking when scores are nearly equal
    private const EPS_TIE = 1.0e-9'f64
    // Scaling factor for edge distance exponential decay
    private const EDGE_DIST_SCALE = 0.15

    // Balanced weights for combining contrast, saturation, and edge information
    private const ENHANCED_WEIGHTS = SaliencyWeights{contrast: 1.0, saturation: 0.55, edge: 1.2}
    // Fast preset optimized for speed over quality
    private const FAST_OPTS = SearchOpts{minCoverage: 0.50, blurRadius: 2, strideFrac: 0.08, thirdsWeight: 0.15, centerWeight: 0.08, shrinkPrior: 0.10, edgePenalty: 0.10, faceWeight: 0.0}

    // Convert sRGB color value to linear RGB for perceptually accurate calculations
    private mtd const srgbToLinear(c: f32)->f32
    {
        if c <= 0.04045:
            return c / 12.92
        return Math.pow((c + 0.055) / 1.055, 2.4)
    }

    // Build an integral image for O(1) rectangular sum queries
    // Each cell (x,y) contains sum of all values in rectangle from (0,0) to (x,y)
    private mtd const buildIntegral(src: &Array'f32, W, H: s32)->Array'f64
    {
        var ii: Array'f64
        ii.resize(cast(u64) ((W + 1) * (H + 1)))  // Extra row/col of zeros for boundary handling
        for y in 1 to H
        {
            var rowSum = 0.0'f64
            for x in 1 to W
            {
                let s = cast(f64) src[cast(u64) ((y - 1) * W + (x - 1))]
                rowSum += s
                let idx = cast(u64) (y * (W + 1) + x)
                ii[idx] = ii[idx - cast(u64) (W + 1)] + rowSum  // Current cell = above + row sum
            }
        }
        return ii
    }

    // Compute sum of values in rectangle [x0,y0) to [x1,y1) using integral image
    // Uses inclusion-exclusion principle: D - B - C + A
    private mtd const sumRect(ii: &Array'f64, W, H, x0, y0, x1, y1: s32)->f64
    {
        let A = y0 * (W + 1) + x0
        let B = y0 * (W + 1) + x1
        let C = y1 * (W + 1) + x0
        let D = y1 * (W + 1) + x1
        return ii[D] - ii[B] - ii[C] + ii[A]
    }

    // Apply box blur using integral image for O(1) per-pixel complexity
    // Averages all pixels within square radius around each pixel
    private mtd const boxBlurViaIntegral(src: &Array'f32, W, H, radius: s32)->Array'f32
    {
        if radius <= 0
        {
            // No blur requested, return copy of source
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
                let n  = cast(f64) ((x1 - x0 + 1) * (y1 - y0 + 1))  // Number of pixels averaged
                dst[cast(u64) (y * W + x)] = cast(f32) (S / n)
            }
        }
        return dst
    }

    // Generate rule-of-thirds weight map: high values at 1/3 and 2/3 positions
    // Based on photographic composition principle that subjects at thirds are more pleasing
    private mtd const buildThirdsMap(W, H: s32, sigma: f32)->Array'f32
    {
        var tx, ty: Array'f32
        tx.resize(cast(u64) W)
        ty.resize(cast(u64) H)

        let t1:  f32 = 1.0 / 3.0
        let t2:  f32 = 2.0 / 3.0
        let sig2 = sigma * sigma

        // Create 1D Gaussian peaks at third positions
        for x in 0 to (W - 1)
        {
            let fx = cast(f32) x / cast(f32) W  // Normalized position
            let dx = Math.min(Math.abs(fx - t1), Math.abs(fx - t2))  // Distance to nearest third
            tx[cast(u64) x] = Math.exp(-(dx * dx) / (2.0 * sig2))
        }
        for y in 0 to (H - 1)
        {
            let fy = cast(f32) y / cast(f32) H
            let dy = Math.min(Math.abs(fy - t1), Math.abs(fy - t2))
            ty[cast(u64) y] = Math.exp(-(dy * dy) / (2.0 * sig2))
        }

        // Combine horizontal and vertical with power for sharper peaks
        var T: Array'f32
        T.resize(cast(u64) (W * H))
        for y in 0 to (H - 1):
            for x in 0 to (W - 1):
                T[cast(u64) (y * W + x)] = Math.pow(tx[cast(u64) x] * ty[cast(u64) y], 1.5)
        return T
    }

    // Compute distance transform from edge pixels
    // Returns 1.0 at edges, smoothly decaying to lower values away from edges
    // Used to penalize crops that cut through detected edges
    private mtd const buildEdgeDistanceMap(edges: &Array'f32, W, H: s32)->Array'f32
    {
        var dist: Array'f32
        dist.resize(cast(u64) (W * H))

        for y in 0 to (H - 1)
        {
            for x in 0 to (W - 1)
            {
                let i = cast(u64) (y * W + x)
                if edges[i] > 0.3  // This pixel is an edge
                {
                    dist[i] = 1.0
                }
                else
                {
                    // Search neighborhood for nearest edge pixel
                    var minD = cast(f32) Math.sqrt(cast(f64) (W * W + H * H))
                    let rad  = 8  // Search within 8-pixel radius
                    for dy in -rad to rad
                    {
                        for dx in -rad to rad
                        {
                            let nx = x + dx
                            let ny = y + dy
                            if nx < 0 or ny < 0 or nx >= W or ny >= H:
                                continue
                            let ni = cast(u64) (ny * W + nx)
                            if edges[ni] > 0.3
                            {
                                let d = cast(f32) Math.sqrt(cast(f64) (dx * dx + dy * dy))
                                minD = Math.min(minD, d)
                            }
                        }
                    }
                    // Exponential decay from edges
                    dist[i] = 1.0 - Math.exp(-EDGE_DIST_SCALE * minD)
                }
            }
        }
        return dist
    }

    // Score a crop window using weighted combination of multiple factors
    // Returns higher score for better crops
    private mtd const scoreWindow(ctx: &ScoreCtx, x0, y0, cw, ch: s32)->{ score: f64, d2: f64 }
    {
        let x1   = x0 + cw
        let y1   = y0 + ch
        let area = cast(f64) (cw * ch)

        // Average saliency in this window
        let sumS = .sumRect(ctx.iiS, ctx.w, ctx.h, x0, y0, x1, y1)
        // Average rule-of-thirds score in this window
        let sumT = .sumRect(ctx.iiT, ctx.w, ctx.h, x0, y0, x1, y1)
        // Average edge distance in this window
        let sumE = .sumRect(ctx.iiE, ctx.w, ctx.h, x0, y0, x1, y1)

        // Base score: saliency + rule of thirds bonus
        var s = (sumS / area) + ctx.thirdsW * (sumT / area)

        // Penalty for cutting through edges (low edge distance means near edges)
        s -= ctx.edgePen * (1.0 - (sumE / area))

        // Penalty for small crops (encourages preserving more of the image)
        let areaFrac = area / ctx.imgArea
        s += ctx.shrinkW * (1.0 - areaFrac)

        // Bonus for crops centered near image center
        let cwx = cast(f64) x0 + 0.5 * cast(f64) cw  // Window center X
        let cwy = cast(f64) y0 + 0.5 * cast(f64) ch  // Window center Y
        let d2  = (cwx - ctx.cx) * (cwx - ctx.cx) + (cwy - ctx.cy) * (cwy - ctx.cy)
        s += ctx.centerW * (1.0 - (d2 / ctx.d2Max))

        return {s, d2}  // Return score and squared distance for tie-breaking
    }

    // Adjust crop dimensions to match target aspect ratio
    // Prioritizes the dimension that's more constrained by the target aspect
    private mtd const fitAspect(cw, ch, W, H: s32, aspect, imgAspect: f32)->{ w: s32, h: s32 }
    {
        var w = cw, h = ch
        if imgAspect > aspect  // Image is wider than target
        {
            // Fix height, adjust width to match aspect
            h = Math.min(H, Math.max(4, h))
            w = Math.min(W, Math.max(4, cast(s32) Math.round(cast(f64) h * cast(f64) aspect)))
        }
        else  // Image is taller than target
        {
            // Fix width, adjust height to match aspect
            w = Math.min(W, Math.max(4, w))
            h = Math.min(H, Math.max(4, cast(s32) Math.round(cast(f64) w / cast(f64) aspect)))
        }
        return {w, h}
    }

    // Fine-tune crop position by testing nearby positions
    // Searches within 'range' pixels in each direction
    private mtd const refinePosition(ctx: &ScoreCtx, bestX, bestY, bestW, bestH: s32, range: s32 = 2)->{ x: s32, y: s32, score: f64, d2: f64 }
    {
        let maxX   = ctx.w - bestW
        let maxY   = ctx.h - bestH
        var bx = bestX, by = bestY
        var bScore = -1.0e30'f64
        var bD2    = 1.0e30'f64

        // Score initial position
        {
            let r0 = .scoreWindow(ctx, bx, by, bestW, bestH)
            bScore = r0.score; bD2    = r0.d2
        }

        // Define search rectangle, clamped to valid positions
        let rx0 = Math.clamp(bestX - range, 0, maxX)
        let ry0 = Math.clamp(bestY - range, 0, maxY)
        let rx1 = Math.clamp(bestX + range, 0, maxX)
        let ry1 = Math.clamp(bestY + range, 0, maxY)

        // Test all positions in range
        for y0 in ry0 to ry1
        {
            for x0 in rx0 to rx1
            {
                let r = .scoreWindow(ctx, x0, y0, bestW, bestH)
                // Update if better score, or same score but more centered
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

    // Try different scale variants of the crop to find optimal size
    // Tests 7 scales: 85%, 91%, 97%, 100%, 103%, 109%, 115%
    private mtd const tryScaledVariants(ctx: &ScoreCtx, aspect, imgAspect: f32, bestX, bestY, bestW, bestH: s32)->{ x: s32, y: s32, w: s32, h: s32, score: f64, d2: f64 }
    {
        let scales: [7] f64 = [0.85, 0.91, 0.97, 1.00, 1.03, 1.09, 1.15]

        var bx = bestX, by = bestY, bw = bestW, bh = bestH
        var bScore = -1.0e30'f64
        var bD2    = 1.0e30'f64

        // Score original size
        {
            let r0 = .scoreWindow(ctx, bestX, bestY, bestW, bestH)
            bScore = r0.score; bD2    = r0.d2
        }

        for i in 0 to 6
        {
            if i == 3:  // Skip 100% since we already tested it
                continue
            var cw = cast(s32) Math.round(cast(f64) bestW * scales[i])
            var ch = cast(s32) Math.round(cast(f64) bestH * scales[i])
            if cw < 4 or ch < 4:
                continue
            if cw > ctx.w or ch > ctx.h:
                continue

            // Adjust to maintain target aspect ratio
            let (fw, fh) = .fitAspect(cw, ch, ctx.w, ctx.h, aspect, imgAspect)
            if fw < 4 or fh < 4:
                continue

            let maxX = ctx.w - fw
            let maxY = ctx.h - fh
            if maxX < 0 or maxY < 0:
                continue

            // Center the scaled crop around original position
            let x0 = Math.clamp(bestX + (bestW - fw) / 2, 0, maxX)
            let y0 = Math.clamp(bestY + (bestH - fh) / 2, 0, maxY)
            let r  = .scoreWindow(ctx, x0, y0, fw, fh)

            // Update if this scale is better
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

    // Fast single-scale cropping algorithm
    // Simpler and faster than smart mode, suitable for real-time applications
    private mtd const calculateReframeRect(targetWidth, targetHeight: f32)->Math.Rectangle
    {
        let aspect    = targetWidth / targetHeight
        let W         = .width
        let H         = .height
        let imgAspect = cast(f32) W / cast(f32) H

        // Determine crop dimensions to match target aspect ratio
        var cropW: s32
        var cropH: s32
        if imgAspect > aspect  // Image wider than target
        {
            cropH = H  // Use full height
            cropW = cast(s32) Math.floor(aspect * cast(f32) cropH + 0.5)
        }
        else  // Image taller than target
        {
            cropW = W  // Use full width
            cropH = cast(s32) Math.floor(cast(f32) cropW / aspect + 0.5)
        }

        // Compute saliency map
        var S: Array'f32
        S.resize(cast(u64) (W * H))
        .computeSaliencyMap(&S, ENHANCED_WEIGHTS)

        // Blur saliency for smoother results
        var Sb = .boxBlurViaIntegral(&S, W, H, FAST_OPTS.blurRadius)

        // Add rule-of-thirds bonus directly to saliency
        var T = .buildThirdsMap(W, H, THIRDS_SIGMA)
        for i in 0 to (W * H - 1)
        {
            let idx = cast(u64) i
            Sb[idx] = Sb[idx] + 0.15 * T[idx]  // 15% rule-of-thirds weight
        }

        // Build integral image for fast region sum queries
        var ii = .buildIntegral(&Sb, W, H)

        var bestX, bestY = 0's32
        var bestSum = -1.0'f64

        let maxX = W - cropW
        let maxY = H - cropH

        let cxImg           = 0.5'f64 * cast(f64) W
        let cyImg           = 0.5'f64 * cast(f64) H
        var bestCenterDist2 = 1.0e30'f64

        // Stride-based coarse search (test every ~5% of crop size)
        let stride = Math.max(1, Math.min(cropW, cropH) / 20)

        for var y0 = 0; y0 <= maxY; y0 += stride
        {
            let y1 = y0 + cropH
            for var x0 = 0; x0 <= maxX; x0 += stride
            {
                let x1  = x0 + cropW
                let sum = .sumRect(&ii, W, H, x0, y0, x1, y1)

                if sum > bestSum + EPS_TIE
                {
                    bestSum = sum; bestX   = x0; bestY   = y0
                    let cwx = cast(f64) x0 + 0.5 * cast(f64) cropW
                    let cwy = cast(f64) y0 + 0.5 * cast(f64) cropH
                    bestCenterDist2 = (cwx - cxImg) * (cwx - cxImg) + (cwy - cyImg) * (cwy - cyImg)
                }
                elif Math.abs(sum - bestSum) <= EPS_TIE  // Tie: prefer more centered
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

        // Fine refinement pass in ±3 pixel neighborhood
        {
            let rx0 = Math.clamp(bestX - 3, 0, maxX)
            let ry0 = Math.clamp(bestY - 3, 0, maxY)
            let rx1 = Math.clamp(bestX + 3, 0, maxX)
            let ry1 = Math.clamp(bestY + 3, 0, maxY)
            for y0 in ry0 to ry1
            {
                for x0 in rx0 to rx1
                {
                    let sum = .sumRect(&ii, W, H, x0, y0, x0 + cropW, y0 + cropH)
                    if sum > bestSum + EPS_TIE
                    {
                        bestSum = sum
                        bestX   = x0
                        bestY   = y0
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

    // Advanced multi-scale cropping algorithm
    // Searches across different crop sizes and refines both position and scale
    private mtd const calculateReframeRectSmart(targetWidth, targetHeight: f32, opts: SearchOpts)->Math.Rectangle
    {
        let W = .width, H = .height
        Debug.assert(W > 0 and H > 0)

        let aspect    = targetWidth / targetHeight
        let imgAspect = cast(f32) W / cast(f32) H
        let imgArea   = cast(f64) (W * H)

        // Compute saliency map
        var S: Array'f32
        S.resize(cast(u64) (W * H))
        .computeSaliencyMap(&S, ENHANCED_WEIGHTS)

        // Apply blur for smoother gradients
        var Sb = .boxBlurViaIntegral(&S, W, H, opts.blurRadius)

        // Build rule-of-thirds map
        var T = .buildThirdsMap(W, H, THIRDS_SIGMA)

        // Build edge map and distance transform
        var edges: Array'f32
        edges.resize(cast(u64) (W * H))
        .computeEdgeMap(&edges)
        var E = .buildEdgeDistanceMap(&edges, W, H)

        // Create integral images for all features
        var iiS = .buildIntegral(&Sb, W, H)
        var iiT = .buildIntegral(&T, W, H)
        var iiE = .buildIntegral(&E, W, H)

        // Prepare scoring context
        let cx   = 0.5'f64 * cast(f64) W
        let cy   = 0.5'f64 * cast(f64) H
        let d2Mx = cast(f64) W * cast(f64) W + cast(f64) H * cast(f64) H

        var ctx: ScoreCtx
        ctx.iiS     = &iiS
        ctx.iiT     = &iiT
        ctx.iiE     = &iiE
        ctx.w       = W
        ctx.h       = H
        ctx.imgArea = imgArea
        ctx.thirdsW = cast(f64) opts.thirdsWeight
        ctx.centerW = cast(f64) opts.centerWeight
        ctx.shrinkW = cast(f64) opts.shrinkPrior
        ctx.edgePen = cast(f64) opts.edgePenalty
        ctx.cx      = cx
        ctx.cy      = cy
        ctx.d2Max   = d2Mx

        let minArea = cast(f64) opts.minCoverage * imgArea
        const STEPS = 16's32  // Test 16 different crop sizes

        var bestX, bestY, bestW, bestH = 0's32
        var bestScore    = -1.0e30'f64
        var bestCenterD2 = 1.0e30'f64

        // Multi-scale search: test progressively smaller crops
        for step in 0 to (STEPS - 1)
        {
            let t          = cast(f64) step / cast(f64) (STEPS - 1)
            // Non-linear progression: more steps near full coverage
            let frac       = 1.0 - Math.pow(t, 1.3) * (1.0 - cast(f64) opts.minCoverage)
            let targetArea = Math.max(frac * imgArea, minArea)

            // Calculate crop dimensions for this area
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

            // Stride proportional to crop size (controlled by strideFrac parameter)
            let strideX = Math.max(1, cast(s32) Math.round(opts.strideFrac * cast(f32) cw))
            let strideY = Math.max(1, cast(s32) Math.round(opts.strideFrac * cast(f32) ch))

            // Search all positions at this scale
            for var y0 = 0; y0 <= maxY; y0 += strideY
            {
                for var x0 = 0; x0 <= maxX; x0 += strideX
                {
                    let r = .scoreWindow(&ctx, x0, y0, cw, ch)
                    if r.score > bestScore + EPS_TIE
                    {
                        bestScore    = r.score
                        bestX        = x0
                        bestY        = y0
                        bestW        = cw
                        bestH        = ch
                        bestCenterD2 = r.d2
                    }
                    elif Math.abs(r.score - bestScore) <= EPS_TIE and r.d2 < bestCenterD2
                    {
                        // Same score, prefer more centered crop
                        bestX        = x0
                        bestY        = y0
                        bestW        = cw
                        bestH        = ch
                        bestCenterD2 = r.d2
                    }
                }
            }
        }

        // Fallback to fast mode if search failed
        if bestW == 0 or bestH == 0:
            return .calculateReframeRect(targetWidth, targetHeight)

        // Multi-pass refinement to find optimal position and scale
        {
            // Refine position at current scale with decreasing ranges
            foreach range in [4, 2, 1]
            {
                let rr = .refinePosition(&ctx, bestX, bestY, bestW, bestH, cast() range)
                if rr.score > bestScore + EPS_TIE
                {
                    bestScore    = rr.score
                    bestX        = rr.x
                    bestY        = rr.y
                    bestCenterD2 = rr.d2
                }
            }

            // Try different scales around current size
            let sr = .tryScaledVariants(&ctx, aspect, imgAspect, bestX, bestY, bestW, bestH)
            if sr.score > bestScore + EPS_TIE
            {
                bestScore    = sr.score
                bestX        = sr.x
                bestY        = sr.y
                bestW        = sr.w
                bestH        = sr.h
                bestCenterD2 = sr.d2
            }

            // Final position refinement at optimal scale
            let rr2 = .refinePosition(&ctx, bestX, bestY, bestW, bestH, 1)
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

    // Estimate percentiles using sampled subset for efficiency
    // Samples up to 4096 values to avoid sorting entire arrays
    private mtd const estimatePercentiles(buf: &Array'f32, N: s32, pLow, pHigh: f32)->{ a: f32, b: f32 }
    {
        const K  = 4096's32  // Maximum sample size
        var tmp: Array'f32
        var M    = Math.min(K, N)
        tmp.resize(cast(u64) M)

        if N <= K
        {
            // Use all values if small enough
            for i in 0 to (N - 1):
                tmp[cast(u64) i] = buf[cast(u64) i]
        }
        else
        {
            // Sample every Nth value
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

        tmp.sort()

        // Find values at percentile positions
        let il = Math.max(0, Math.min(M - 1, cast(s32) Math.floor(pLow * cast(f32) (M - 1))))
        let ih = Math.max(0, Math.min(M - 1, cast(s32) Math.floor(pHigh * cast(f32) (M - 1))))
        let lo = tmp[cast(u64) il]
        let hi = tmp[cast(u64) ih]
        if hi <= lo:
            return {lo, lo + 1e-6}  // Avoid division by zero
        return {lo, hi}
    }

    // Compute per-pixel saliency based on local contrast, saturation, and edges
    // Saliency indicates visual importance for determining crop regions
    private mtd const computeSaliencyMap(saliencyMap: &Array'f32, weights: SaliencyWeights)
    {
        let W = .width
        let H = .height
        let N = cast(s32) (W * H)

        // Extract luminance and saturation for each pixel
        var lum, sat: Array'f32
        lum.resize(cast(u64) N)
        sat.resize(cast(u64) N)

        var f   = {lum: &lum, sat: &sat}
        let mec = cast #unconst (*Image) me
        mec.visitPixels(&f)
        {
            let p      = image.getPixelColor(x, y)
            // Convert to linear RGB for perceptually accurate luminance
            let r_lin  = image.srgbToLinear(p.r / 255.0)
            let g_lin  = image.srgbToLinear(p.g / 255.0)
            let b_lin  = image.srgbToLinear(p.b / 255.0)
            // Rec. 709 luminance weights
            let L      = r_lin * 0.2126 + g_lin * 0.7152 + b_lin * 0.0722
            let params = cast(*#decltype(f)) userData
            params.lum.buffer[index] = L

            // Calculate saturation in sRGB space
            let rs   = p.r / 255.0
            let gs   = p.g / 255.0
            let bs   = p.b / 255.0
            let maxC = Math.max(Math.max(rs, gs), bs)
            let minC = Math.min(Math.min(rs, gs), bs)
            let eps  = 1e-6'f32
            params.sat.buffer[index] = (maxC > 0.0) ? ((maxC - minC) / (maxC + eps)) : 0.0
        }

        // Build integral images for fast local statistics
        var ii, iiSq: Array'f64
        ii.resize(cast(u64) ((W + 1) * (H + 1)))
        iiSq.resize(cast(u64) ((W + 1) * (H + 1)))
        for y in 1 to H
        {
            var rowSum = 0.0'f64, rowSumSq = 0.0'f64
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

        // Blur luminance for edge detection
        var lumBlur = .boxBlurViaIntegral(&lum, W, H, 1)
        var edge:   Array'f32
        edge.resize(cast(u64) N)

        // Compute edge strength using Sobel operator
        for y in 0 to (H - 1)
        {
            for x in 0 to (W - 1)
            {
                let i = cast(u64) (y * W + x)
                if x == 0 or y == 0 or x == W - 1 or y == H - 1
                {
                    edge[i] = 0.0  // No edges at borders
                    continue
                }

                // 3x3 Sobel kernel neighbors
                let Lm1m1 = lumBlur[((y - 1) * W + (x - 1))]
                let Lp1m1 = lumBlur[((y - 1) * W + (x + 1))]
                let Lm10  = lumBlur[(y * W + (x - 1))]
                let Lp10  = lumBlur[(y * W + (x + 1))]
                let Lm1p1 = lumBlur[((y + 1) * W + (x - 1))]
                let Lp1p1 = lumBlur[((y + 1) * W + (x + 1))]
                let L0m1  = lumBlur[((y - 1) * W + x)]
                let L0p1  = lumBlur[((y + 1) * W + x)]

                // Sobel X and Y gradients
                let gx = -Lm1m1 + Lp1m1 - 2.0 * Lm10 + 2.0 * Lp10 - Lm1p1 + Lp1p1
                let gy = -Lm1m1 - 2.0 * L0m1 - Lp1m1 + Lm1p1 + 2.0 * L0p1 + Lp1p1
                edge[i] = cast(f32) Math.sqrt(gx * gx + gy * gy)
            }
        }

        // Compute local contrast using variance in neighborhood
        const R       = SALIENCY_RADIUS
        var contrast: Array'f32
        contrast.resize(cast(u64) N)

        for y in 0 to (H - 1)
        {
            for x in 0 to (W - 1)
            {
                let i  = cast(u64) (y * W + x)
                let x0 = Math.max(0, x - R)
                let y0 = Math.max(0, y - R)
                let x1 = Math.min(W - 1, x + R)
                let y1 = Math.min(H - 1, y + R)

                // Query integral images for rectangle stats
                let A = cast(u64) (y0 * (W + 1) + x0)
                let B = cast(u64) (y0 * (W + 1) + (x1 + 1))
                let C = cast(u64) ((y1 + 1) * (W + 1) + x0)
                let D = cast(u64) ((y1 + 1) * (W + 1) + (x1 + 1))

                let n     = cast(f64) ((x1 - x0 + 1) * (y1 - y0 + 1))
                let sum   = ii[D] - ii[B] - ii[C] + ii[A]
                let sumSq = iiSq[D] - iiSq[B] - iiSq[C] + iiSq[A]

                // Variance = E[X²] - E[X]²
                let mean = sum / n
                let ovar = Math.max(0.0, (sumSq / n) - mean * mean)
                contrast[i] = cast(f32) Math.sqrt(ovar)  // Standard deviation
            }
        }

        // Normalize each feature to [0,1] using percentiles (robust to outliers)
        let (cLo, cHi) = .estimatePercentiles(&contrast, N, 0.01, 0.99)
        let (sLo, sHi) = .estimatePercentiles(&sat, N, 0.01, 0.99)
        let (eLo, eHi) = .estimatePercentiles(&edge, N, 0.01, 0.99)

        let cDen = Math.max(1e-6, cHi - cLo)
        let sDen = Math.max(1e-6, sHi - sLo)
        let eDen = Math.max(1e-6, eHi - eLo)

        // Combine features with power curves for perceptual tuning
        for i in 0 to (N - 1)
        {
            let cn = Math.saturate((contrast[cast(u64) i] - cLo) / cDen)
            let sn = Math.saturate((sat[cast(u64) i] - sLo) / sDen)
            let en = Math.saturate((edge[cast(u64) i] - eLo) / eDen)

            // Power curves: contrast^0.9, saturation^1.1, edge^0.85
            let combined = weights.contrast * Math.pow(cn, 0.9) +
                           weights.saturation * Math.pow(sn, 1.1) +
                           weights.edge * Math.pow(en, 0.85)

            saliencyMap[i] = combined
        }
    }

    // Compute edge magnitude map for edge-aware cropping
    private mtd const computeEdgeMap(edgeMap: &Array'f32)
    {
        let W = .width
        let H = .height

        // Extract luminance
        var lum: Array'f32
        lum.resize(cast(u64) (W * H))

        var f   = {lum: &lum}
        let mec = cast #unconst (*Image) me
        mec.visitPixels(&f)
        {
            let p      = image.getPixelColor(x, y)
            let r_lin  = image.srgbToLinear(p.r / 255.0)
            let g_lin  = image.srgbToLinear(p.g / 255.0)
            let b_lin  = image.srgbToLinear(p.b / 255.0)
            let L      = r_lin * 0.2126 + g_lin * 0.7152 + b_lin * 0.0722
            let params = cast(*#decltype(f)) userData
            params.lum.buffer[index] = L
        }

        var lumBlur = .boxBlurViaIntegral(&lum, W, H, 1)

        // Sobel edge detection
        for y in 0 to (H - 1)
        {
            for x in 0 to (W - 1)
            {
                let i = cast(u64) (y * W + x)
                if x == 0 or y == 0 or x == W - 1 or y == H - 1
                {
                    edgeMap[i] = 0.0
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

                let gx  = -Lm1m1 + Lp1m1 - 2.0 * Lm10 + 2.0 * Lp10 - Lm1p1 + Lp1p1
                let gy  = -Lm1m1 - 2.0 * L0m1 - Lp1m1 + Lm1p1 + 2.0 * L0p1 + Lp1p1
                let mag = cast(f32) Math.sqrt(gx * gx + gy * gy)

                edgeMap[i] = Math.min(1.0, mag * 0.5)
            }
        }
    }

    // Find the most salient (visually important) point in the image
    private mtd const findInterestPoint()->InterestPoint
    {
        var saliencyMap: Array'f32
        saliencyMap.resize(cast(u64) (.width * .height))
        .computeSaliencyMap(&saliencyMap, ENHANCED_WEIGHTS)

        var maxScore = -1.0'f32
        var maxX, maxY = 0's32

        // Find pixel with maximum saliency
        for y in 0 to (.height - 1)
        {
            for x in 0 to (.width - 1)
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
        out.x     = cast(f32) maxX / cast(f32) .width   // Normalized X coordinate
        out.y     = cast(f32) maxY / cast(f32) .height  // Normalized Y coordinate
        out.score = maxScore
        return out
    }
}

public impl Image
{
    // Smart crop search parameters
    struct SearchOpts
    {
        // Minimum fraction of original image to preserve (range: 0.0-1.0, typically 0.4-0.7)
        // Lower values allow tighter crops, higher values preserve more context
        // Example: 0.4 = crop can be as small as 40% of original image area
        minCoverage:      f32 = 0.4

        // Blur radius for smoothing saliency map (range: 0-5 pixels)
        // Higher values create smoother saliency gradients but lose detail
        // 0 = no blur, 2 = fast/subtle, 3 = balanced, 5 = very smooth
        blurRadius:       s32 = 3

        // Stride fraction for position search (range: 0.01-0.15)
        // Controls spacing between tested positions as fraction of crop size
        // Lower = more thorough but slower, higher = faster but may miss optimal position
        // Example: 0.04 = test positions every 4% of crop dimension
        strideFrac:       f32 = 0.04

        // Weight for rule-of-thirds bonus (range: 0.0-0.5, typically 0.15-0.25)
        // Higher values strongly favor crops aligned with compositional thirds
        // 0.0 = ignore rule of thirds, 0.22 = balanced, 0.5 = very strong preference
        thirdsWeight:     f32 = 0.22

        // Weight for center proximity bonus (range: 0.0-0.3, typically 0.05-0.15)
        // Higher values favor crops centered on the image center
        // 0.0 = no center bias, 0.06 = subtle, 0.15 = strong center preference
        centerWeight:     f32 = 0.06

        // Prior favoring larger crops (range: 0.0-0.3, typically 0.05-0.15)
        // Penalizes small crops to preserve more context when possible
        // 0.0 = no size preference, 0.08 = subtle, 0.2 = strong preference for full image
        shrinkPrior:      f32 = 0.08

        // Penalty for cutting through edges (range: 0.0-0.5, typically 0.1-0.25)
        // Higher values avoid cropping that slices through detected edges
        // 0.0 = ignore edges, 0.15 = balanced, 0.3 = strongly avoid edge cuts
        edgePenalty:      f32 = 0.15

        // Weight for face detection bonus (range: 0.0-1.0, currently unused)
        // Reserved for future face detection integration
        // Would increase score for crops containing detected faces
        faceWeight:       f32 = 0.0
    }

    // Perform content-aware smart crop to target rectangle
    // rect: Target dimensions (width and height define aspect ratio and reference size)
    // enableScaling: If true, uses advanced multi-scale algorithm; if false, uses fast single-scale
    mtd smartCrop(rect: Math.Rectangle, enableScaling: bool = false)
    {
        var opts: SearchOpts
        Debug.assert(rect.width > 0 and rect.height > 0)
        if enableScaling:
            .crop(.calculateReframeRectSmart(rect.width, rect.height, opts))
        else:
            .crop(.calculateReframeRect(rect.width, rect.height))
    }
}