using Core

public impl Image
{
    enum ResizeMode
    {
        Raw
        Bilinear
        Bicubic
        Gaussian
        Quadratic
        Hermite
        Hamming
        Catrom
    }

    // Resize image
    // Put 'newWidth' or 'newHeight' to 0 to keep aspect ratio
    mtd resize(newWidth, newHeight: s32, mode = ResizeMode.Raw)
    {
        Debug.assert(newWidth > 0 or newHeight > 0)
        let nw = newWidth orelse cast(s32) (newHeight * (.width / cast(f32) .height))
        let nh = newHeight orelse cast(s32) (newWidth * (.height / cast(f32) .width))
        if nw == .width and nh == .height do
            return
        .allocWorkingBuffer(nw, nh, .pf)
        ResizeTools.doResize(.workingBuffer, me, mode)
        .applyWorkingBuffer()
    }

    // Resize image
    // Put 'newWidth' or 'newHeight' to 0 to keep aspect ratio
    mtd const toResize(newWidth, newHeight: s32, mode = ResizeMode.Raw)->Image
    {
        Debug.assert(newWidth > 0 or newHeight > 0)
        let nw = newWidth orelse cast(s32) (newHeight * (.width / cast(f32) .height))
        let nh = newHeight orelse cast(s32) (newWidth * (.height / cast(f32) .width))
        if nw == .width and nh == .height
        {
            var result: retval = dref me
            return result
        }

        using ResizeTools
        var result: retval = create(nw, nh, .pf)
        ResizeTools.doResize(&result, me, mode)
        return result
    }
}

namespace ResizeTools
{
    struct Contribution
    {
        weights:         Array'f64
        left, right:     s64
    }

    struct LineContrib
    {
        row:            Array'Contribution
        windowSize:     s64
        lineLength:     s64
    }

    func allocContributions(lineLength, windowSize: s64)->LineContrib
    {
        var res: retval
        res.windowSize = windowSize
        res.lineLength = lineLength
        res.row.resize(cast(u64) lineLength)
        for i in lineLength do
            res.row[i].weights.resize(cast(u64) windowSize)
        return res
    }

    func getFilterWidth(rmode: Image.ResizeMode)->f64
    {
        switch rmode
        {
        case Bilinear:
            return 1
        case Hermite:
            return 1
        case Hamming:
            return 1
        case Quadratic:
            return 1.5
        case Bicubic:
            return 2
        case Catrom:
            return 2
        case Gaussian:
            return 3
        }

        return 0
    }

    func getFilter(resf: f64, rmode: Image.ResizeMode)->f64
    {
        switch rmode
        {
        case Bilinear:
            let res = Math.abs(resf)
            return res < 1 ? 1 - res : 0
        case Bicubic:
            if resf < -2.0 do
                return 0
            if resf < -1.0 do
                return ((2.0 + resf) * (2.0 + resf) * (2.0 + resf) / 6.0)
            if resf < 0.0 do
                return ((4.0 + resf * resf * (-6.0 - 3.0 * resf)) / 6.0)
            if resf < 1.0 do
                return ((4.0 + resf * resf * (-6.0 + 3.0 * resf)) / 6.0)
            if resf < 2.0 do
                return ((2.0 - resf) * (2.0 - resf) * (2.0 - resf) / 6.0)
            return 0
        case Gaussian:
            if Math.abs(resf) > 3.0 do
                return 0
            return Math.exp(-resf * resf * 0.5) / 0.79788456080287
        case Hermite:
            if resf < -1.0 do
                return 0
            if resf < 0.0 do
                return (-2.0 * resf - 3.0) * resf * resf + 1.0
            if resf < 1.0 do
                return (2.0 * resf - 3.0) * resf * resf + 1.0
            return 0
        case Hamming:
            if resf < -1.0 do
                return 0.0
            if resf < 0.0 do
                return 0.92 * (-2.0 * resf - 3.0) * resf * resf + 1.0
            if resf < 1.0 do
                return 0.92 * (2.0 * resf - 3.0) * resf * resf + 1.0
            return 0
        case Catrom:
            if resf < -2.0 do
                return 0
            if resf < -1.0 do
                return (0.5 * (4.0 + resf * (8.0 + resf * (5.0 + resf))))
            if resf < 0.0 do
                return (0.5 * (2.0 + resf * resf * (-5.0 - 3.0 * resf)))
            if resf < 1.0 do
                return (0.5 * (2.0 + resf * resf * (-5.0 + 3.0 * resf)))
            if resf < 2.0 do
                return (0.5 * (4.0 + resf * (-8.0 + resf * (5.0 - resf))))
            return 0
        case Quadratic:
            if resf < -1.5 do
                return 0.0
            if resf < -0.5 do
                return (0.5 * (resf + 1.5) * (resf + 1.5))
            if resf < 0.5 do
                return (0.75 - resf * resf)
            if resf < 1.5 do
                return (0.5 * (resf - 1.5) * (resf - 1.5))
            return 0
        }

        return 0
    }

    func calcContributions(lineSize, srcSize: s64, scale: f64, rmode: Image.ResizeMode)->LineContrib
    {
        let filterWidth = getFilterWidth(rmode)

        var cscale, width: f64 = undefined
        if scale < 1.0
        {
            width  = filterWidth / scale
            cscale = scale
        }
        else
        {
            width  = filterWidth
            cscale = 1.0
        }

        let windowSize = 2 * (cast(s64) Math.ceil(width) + 1)

        var res: retval
        res = allocContributions(lineSize, windowSize)

        for u in lineSize
        {
            let center = u / scale

            var left  = cast(u64) Math.max(cast(s64) Math.floor(center - width), 0)
            var right = cast(u64) Math.min(cast(s64) Math.ceil(center + width), srcSize - 1)

            // Cut edge points to fit in filter window in case of spill-off
            if right - left + 1 > windowSize
            {
                if left < (srcSize - 1) / 2 do
                    left += 1
                else do
                    right -= 1
            }

            // Compute weights
            var nonZeroFound = false
            var totalWeight  = 0'f64
            for var src = left; src <= right; src += 1
            {
                var resf = getFilter(cscale * (src - center), rmode)
                if resf > 0 do
                    resf *= cscale
                else
                {
                    resf = 0

                    // We are on the left side, trim left
                    if !nonZeroFound
                    {
                        left = src + 1
                        continue
                    }

                    // We are on the right side, trim right
                    right = src - 1
                    break
                }

                nonZeroFound                   = true
                res.row[u].weights[src - left] = resf
                totalWeight += resf
            }

            // Normalize weight of neighboring points
            if totalWeight != 0
            {
                for src in (right - left) + 1 do
                    res.row[u].weights[src] /= totalWeight
            }

            res.row[u].left  = left
            res.row[u].right = right
        }

        return res
    }

    #[Swag.Safety(.All, .All, false), Swag.Overload]
    func scaleRow(imgDst: *Image, imgSrc: const *Image, row: s64, contrib: *LineContrib)
    {
        let bpp8   = imgDst.bpp8
        var dstRow = imgDst.pixels.buffer + row * bpp8 * imgDst.width
        let srcRow = imgSrc.pixels.buffer + row * bpp8 * imgSrc.width

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            let crow0 = contrib.row.buffer
            for x in imgDst.width
            {
                let crow  = crow0 + x
                let left  = crow.left
                let right = crow.right

                #if BPP == 32 do
                    var da: f64
                var dr, dg, db: f64
                var pw  = crow.weights.buffer
                var ptr = srcRow + left * bpp8
                for (right - left) + 1
                {
                    let f = dref pw
                    db += f * ptr[0]
                    dg += f * ptr[1]
                    dr += f * ptr[2]
                    #if BPP == 32 do
                        da += f * ptr[3]
                    pw += 1
                    ptr += bpp8
                }

                dstRow[0] = cast(u8) Math.round(db)
                dstRow[1] = cast(u8) Math.round(dg)
                dstRow[2] = cast(u8) Math.round(dr)
                #if BPP == 32 do
                    dstRow[3] = cast(u8) Math.round(da)
                dstRow += bpp8
            }
        }

        switch imgSrc.bpp
        {
        case 24:
            doIt'24()
        case 32:
            doIt'32()
        default:
            Debug.assert(false)
        }
    }

    #[Swag.Safety(.All, .All, false), Swag.Overload]
    func scaleCol(imgDst: *Image, imgSrc: const *Image, col: s64, contrib: *LineContrib)
    {
        let bpp8   = imgDst.bpp8
        var dstCol = imgDst.pixels.buffer + col * bpp8
        let srcCol = imgSrc.pixels.buffer + col * bpp8
        let w8Dst  = imgDst.width * bpp8
        let w8Src  = imgSrc.width * bpp8

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            let crow0 = contrib.row.buffer
            for y in imgDst.height
            {
                let crow  = crow0 + y
                let left  = crow.left
                let right = crow.right

                #if BPP == 32 do
                    var da: f64
                var dr, dg, db: f64
                var pw  = crow.weights.buffer
                var ptr = srcCol + left * w8Src
                for (right - left) + 1
                {
                    let f = dref pw
                    db += f * ptr[0]
                    dg += f * ptr[1]
                    dr += f * ptr[2]
                    #if BPP == 32 do
                        da += f * ptr[3]
                    pw += 1
                    ptr += cast(u64) w8Src
                }

                dstCol[0] = cast(u8) Math.round(db)
                dstCol[1] = cast(u8) Math.round(dg)
                dstCol[2] = cast(u8) Math.round(dr)
                #if BPP == 32 do
                    dstCol[3] = cast(u8) Math.round(da)
                dstCol += cast(u64) w8Dst
            }
        }

        switch imgSrc.bpp
        {
        case 24:
            doIt'24()
        case 32:
            doIt'32()
        default:
            Debug.assert(false)
        }
    }

    #[Swag.Overload]
    func scaleCol(imgDst: *Image, imgSrc: const *Image, rmode: Image.ResizeMode)
    {
        let sh      = cast(s64) imgSrc.height
        let dh      = cast(s64) imgDst.height
        var contrib = calcContributions(dh, sh, cast(f64) dh / sh, rmode)

        var f = {imgDst, imgSrc, contrib: &contrib}
        Jobs.parallelFor(|index, data| cast(u32) imgDst.width, &f)
        {
            if !data do
                return
            let params = cast(*#decltype(f)) data
            scaleCol(params.imgDst, params.imgSrc, cast(s64) index, params.contrib)
        }
    }

    #[Swag.Overload]
    func scaleRow(imgDst: *Image, imgSrc: const *Image, rmode: Image.ResizeMode)
    {
        let sw      = cast(s64) imgSrc.width
        let dw      = cast(s64) imgDst.width
        var contrib = calcContributions(dw, sw, cast(f64) dw / sw, rmode)

        var f = {imgDst, imgSrc, contrib: &contrib}
        Jobs.parallelFor(|index, data| cast(u32) imgDst.height, &f)
        {
            if !data do
                return
            let params = cast(*#decltype(f)) data
            scaleRow(params.imgDst, params.imgSrc, cast(s64) index, params.contrib)
        }
    }

    func doResizeFilter(dstImage: *Image, srcImage: const *Image, mode: Image.ResizeMode)
    {
        var tmp = Image.create(dstImage.width, srcImage.height, srcImage.pf)
        scaleRow(&tmp, srcImage, mode)
        scaleCol(dstImage, &tmp, mode)
    }

    func doResizeRaw(dstImage: *Image, srcImage: const *Image)
    {
        let xscale = srcImage.width / cast(f32) dstImage.width
        let yscale = srcImage.height / cast(f32) dstImage.height
        var f      = {xscale, yscale, srcImage}

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            dstImage.visitPixels(&f)
            {
                let params = cast(*#decltype(f)) userData
                let fx     = cast(u32) (x * params.xscale)
                let fy     = cast(u32) (y * params.yscale)
                let pixSrc = params.srcImage.pixels.buffer + (fx * image.bpp8) + (fy * params.srcImage.width * image.bpp8)
                Image.copyPixelDirect'BPP(pix, pixSrc)
            }
        }

        switch srcImage.bpp
        {
        case 24:
            doIt'24()
        case 32:
            doIt'32()
        default:
            Debug.assert(false)
        }
    }

    func doResize(dstImage: *Image, srcImage: const *Image, mode: Image.ResizeMode)
    {
        switch mode
        {
        case Raw:
            doResizeRaw(dstImage, srcImage)
        default:
            doResizeFilter(dstImage, srcImage, mode)
        }
    }
}
