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
        nw := newWidth orelse cast(s32) (newHeight * (width / cast(f32) height))
        nh := newHeight orelse cast(s32) (newWidth * (height / cast(f32) width))
        if nw == width and nh == height
            return
        allocWorkingBuffer(nw, nh, pf)
        ResizeTools.doResize(workingBuffer, self, mode)
        applyWorkingBuffer()
    }

    // Resize image
    // Put 'newWidth' or 'newHeight' to 0 to keep aspect ratio
    mtd const toResize(newWidth, newHeight: s32, mode = ResizeMode.Raw)->Image
    {
        Debug.assert(newWidth > 0 or newHeight > 0)
        nw := newWidth orelse cast(s32) (newHeight * (width / cast(f32) height))
        nh := newHeight orelse cast(s32) (newWidth * (height / cast(f32) width))
        if nw == width and nh == height
        {
            var result: retval = dref self
            return result
        }

        using ResizeTools
        var result: retval = create(nw, nh, pf)
        ResizeTools.doResize(&result, self, mode)
        return result
    }
}

namespace ResizeTools
{
    struct Contribution
    {
        weights:        Array'f64
        left, right:    s64
    }

    struct LineContrib
    {
        row:        Array'Contribution
        windowSize: s64
        lineLength: s64
    }

    func allocContributions(lineLength, windowSize: s64)->LineContrib
    {
        var res: retval
        res.windowSize = windowSize
        res.lineLength = lineLength
        res.row.resize(cast(u64) lineLength)
        loop i: lineLength
            res.row[i].weights.resize(cast(u64) windowSize)
        return res;
    }

    func getFilterWidth(rmode: Image.ResizeMode)->f64
    {
        switch rmode
        {
        case Bilinear:  return 1
        case Hermite:   return 1
        case Hamming:   return 1
        case Quadratic: return 1.5
        case Bicubic:   return 2
        case Catrom:    return 2
        case Gaussian:  return 3
        }

        return 0
    }

    func getFilter(resf: f64, rmode: Image.ResizeMode)->f64
    {
        switch rmode
        {
        case Bilinear:
            res := Math.abs(resf)
            return res < 1 ? 1 - res : 0
        case Bicubic:
            if resf < -2.0 return 0
            if resf < -1.0 return ((2.0 + resf) * (2.0 + resf) * (2.0 + resf) / 6.0)
            if resf < 0.0  return ((4.0 + resf * resf * (-6.0 - 3.0 * resf)) / 6.0)
            if resf < 1.0  return ((4.0 + resf * resf * (-6.0 + 3.0 * resf)) / 6.0)
            if resf < 2.0  return ((2.0 - resf) * (2.0 - resf) * (2.0 - resf) / 6.0)
            return 0
        case Gaussian:
            if Math.abs(resf) > 3.0 return 0
            return Math.exp(-resf * resf * 0.5) / 0.79788456080287
        case Hermite:
            if resf < -1.0 return 0
            if resf < 0.0  return (-2.0 * resf - 3.0) * resf * resf + 1.0
            if resf < 1.0  return (2.0 * resf - 3.0) * resf * resf + 1.0
            return 0
        case Hamming:
            if resf < -1.0 return 0.0
            if resf < 0.0  return 0.92 * (-2.0 * resf - 3.0) * resf * resf + 1.0
            if resf < 1.0  return 0.92 * (2.0 * resf - 3.0) * resf * resf + 1.0
            return 0
        case Catrom:
            if resf < -2.0 return 0
            if resf < -1.0 return (0.5 * (4.0 + resf * (8.0 + resf * (5.0 + resf))))
            if resf < 0.0  return (0.5 * (2.0 + resf * resf * (-5.0 - 3.0 * resf)))
            if resf < 1.0  return (0.5 * (2.0 + resf * resf * (-5.0 + 3.0 * resf)))
            if resf < 2.0  return (0.5 * (4.0 + resf * (-8.0 + resf * (5.0 - resf))))
            return 0
        case Quadratic:
            if resf < -1.5 return 0.0
            if resf < -0.5 return (0.5 * (resf + 1.5) * (resf + 1.5))
            if resf < 0.5  return (0.75 - resf * resf)
            if resf < 1.5  return (0.5 * (resf - 1.5) * (resf - 1.5))
            return 0
        }

        return 0
    }

    func calcContributions(lineSize, srcSize: s64, scale: f64, rmode: Image.ResizeMode)->LineContrib
    {
        filterWidth := getFilterWidth(rmode)

        var cscale, width: f64 = undefined
        if scale < 1.0
        {
            width = filterWidth / scale
            cscale = scale
        }
        else
        {
            width = filterWidth
            cscale = 1.0
        }

        windowSize := 2 * (cast(s64) Math.ceil(width) + 1)

        var res: retval
        res = allocContributions(lineSize, windowSize)

        loop u: lineSize
        {
            center := u / scale

            left  := Math.max(cast(s64) Math.floor(center - width), 0)
            right := Math.min(cast(s64) Math.ceil(center + width), srcSize - 1)

            // Cut edge points to fit in filter window in case of spill-off
            if right - left + 1 > windowSize
            {
                if left < (srcSize - 1) / 2
                    left += 1
                else
                    right -= 1
            }

            // Compute weights
            nonZeroFound := false
            totalWeight  := 0'f64
            for src := left; src <= right; src += 1
            {
                resf := getFilter(cscale * (src - center), rmode)
                if (resf > 0)
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

                nonZeroFound = true
                res.row[u].weights[src - left] = resf
                totalWeight += resf
            }

            // Normalize weight of neighboring points
            if totalWeight != 0
            {
                loop src: (right - left) + 1
                    res.row[u].weights[src] /= totalWeight
            }

            res.row[u].left  = left
            res.row[u].right = right
        }

        return res
    }

    #[Swag.Safety("", false)]
    func scaleRow(imgDst: *Image, imgSrc: const *Image, row: s64, contrib: *LineContrib)
    {
        bpp8   := imgDst.bpp8
        dstRow := imgDst.pixels.buffer + row * bpp8 * imgDst.width
        srcRow := imgSrc.pixels.buffer + row * bpp8 * imgSrc.width

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            crow0 := contrib.row.buffer
            loop x: imgDst.width
            {
                crow  := crow0 + x
                left  := crow.left
                right := crow.right

                #if BPP == 32 var da: f64
                var dr, dg, db: f64
                pw  := crow.weights.buffer
                ptr := srcRow + left * bpp8
                loop (right - left) + 1
                {
                    f := dref pw
                    db += f * ptr[0]
                    dg += f * ptr[1]
                    dr += f * ptr[2]
                    #if BPP == 32 da += f * ptr[3]
                    pw += 1
                    ptr += bpp8
                }

                dstRow[0] = cast(u8) db
                dstRow[1] = cast(u8) dg
                dstRow[2] = cast(u8) dr
                #if BPP == 32 dstRow[3] = cast(u8) da
                dstRow += bpp8
            }
        }

        switch imgSrc.bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: Debug.assert(false)
        }
    }

    #[Swag.Safety("", false)]
    func scaleCol(imgDst: *Image, imgSrc: const *Image, col: s64, contrib: *LineContrib)
    {
        bpp8   := imgDst.bpp8
        dstCol := imgDst.pixels.buffer + col * bpp8
        srcCol := imgSrc.pixels.buffer + col * bpp8
        w8Dst  := imgDst.width * bpp8
        w8Src  := imgSrc.width * bpp8

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            crow0 := contrib.row.buffer
            loop y: imgDst.height
            {
                crow  := crow0 + y
                left  := crow.left
                right := crow.right

                #if BPP == 32 var da: f64
                var dr, dg, db: f64
                pw  := crow.weights.buffer
                ptr := srcCol + left * w8Src
                loop (right - left) + 1
                {
                    f := dref pw
                    db += f * ptr[0]
                    dg += f * ptr[1]
                    dr += f * ptr[2]
                    #if BPP == 32 da += f * ptr[3]
                    pw += 1
                    ptr += cast(u64) w8Src
                }

                dstCol[0] = cast(u8) db
                dstCol[1] = cast(u8) dg
                dstCol[2] = cast(u8) dr
                #if BPP == 32 dstCol[3] = cast(u8) da
                dstCol += cast(u64) w8Dst
            }
        }

        switch imgSrc.bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: Debug.assert(false)
        }
    }

    func scaleCol(imgDst: *Image, imgSrc: const *Image, rmode: Image.ResizeMode)
    {
        sh := cast(s64) imgSrc.height
        dh := cast(s64) imgDst.height
        contrib := calcContributions(dh, sh, cast(f64) dh / sh, rmode)

        f := {imgDst, imgSrc, contrib = &contrib}
        Jobs.parallelFor(|index, data| cast(u32) imgDst.width, &f)
        {
            params := cast(*@decltype(f)) data
            scaleCol(params.imgDst, params.imgSrc, cast(s64) index, params.contrib)
        }
    }

    func scaleRow(imgDst: *Image, imgSrc: const *Image, rmode: Image.ResizeMode)
    {
        sw := cast(s64) imgSrc.width
        dw := cast(s64) imgDst.width
        contrib := calcContributions(dw, sw, cast(f64) dw / sw, rmode)

        f := {imgDst, imgSrc, contrib = &contrib}
        Jobs.parallelFor(|index, data| cast(u32) imgDst.height, &f)
        {
            params := cast(*@decltype(f)) data
            scaleRow(params.imgDst, params.imgSrc, cast(s64) index, params.contrib)
        }
    }

    func doResizeFilter(dstImage: *Image, srcImage: const *Image, mode: Image.ResizeMode)
    {
        tmp := Image.create(dstImage.width, srcImage.height, srcImage.pf)
        scaleRow(&tmp, srcImage, mode)
        scaleCol(dstImage, &tmp, mode)
    }

    func doResizeRaw(dstImage: *Image, srcImage: const *Image)
    {
        xscale := srcImage.width / cast(f32) dstImage.width
        yscale := srcImage.height / cast(f32) dstImage.height
        f := {xscale, yscale, srcImage}

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            dstImage.visitPixels(&f)
            {
                params := cast(*@decltype(f)) userData
                fx := cast(u32) (x * params.xscale)
                fy := cast(u32) (y * params.yscale)
                pixSrc := params.srcImage.pixels.buffer + (fx * image.bpp8) + (fy * params.srcImage.width * image.bpp8)
                Image.copyPixel'BPP(pix, pixSrc)
            }
        }

        switch srcImage.bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: Debug.assert(false)
        }
    }

    func doResize(dstImage: *Image, srcImage: const *Image, mode: Image.ResizeMode)
    {
        switch mode
        {
        case Raw:   doResizeRaw(dstImage, srcImage)
        default:    doResizeFilter(dstImage, srcImage, mode)
        }
    }
}