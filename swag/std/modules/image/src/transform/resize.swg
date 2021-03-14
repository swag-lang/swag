using core

public impl Image
{
    enum ResizeMethod
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
    func resize(using self, newWidth, newHeight: u32, method = ResizeMethod.Raw)
    {
        debug.assert(newWidth > 0 && newHeight > 0)
        if newWidth == width && newHeight == height return
        allocWorkingBuffer(newWidth, newHeight, pf)
        doResize(self, workingBuffer, method)
        applyWorkingBuffer()
    }

    // Resize image
    func toResize(using self, newWidth, newHeight: u32, method = ResizeMethod.Raw)->Image
    {
        debug.assert(newWidth > 0 && newHeight > 0)

        if newWidth == width && newHeight == height
        {
            var result: retval = :self
            return result
        }

        var result: retval = create(newWidth, newHeight, pf)
        doResize(self, &result, method)
        return result
    }
}

private
{
    struct Contribution
    {
        weights:        Array'f64
        left, right:    int
    }

    struct LineContrib
    {
        row:        Array'Contribution
        windowSize: int
        lineLength: int
    }

    func allocContributions(lineLength, windowSize: int)->LineContrib
    {
        var res: retval
        res.windowSize = windowSize
        res.lineLength = lineLength
        res.row.resize(cast(uint) lineLength)
        loop i: lineLength
            res.row[i].weights.resize(cast(uint) windowSize)
        return res;
    }

    func getFilterWidth(rmode: Image.ResizeMethod)->f64
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

    func getFilter(resf: f64, rmode: Image.ResizeMethod)->f64
    {
        switch rmode
        {
        case Bilinear:
            res := math.abs(resf)
            return res < 1 ? 1 - res : 0
        case Bicubic:
            if resf < -2.0 return 0
            if resf < -1.0 return ((2.0 + resf) * (2.0 + resf) * (2.0 + resf) / 6.0)
            if resf < 0.0  return ((4.0 + resf * resf * (-6.0 - 3.0 * resf)) / 6.0)
            if resf < 1.0  return ((4.0 + resf * resf * (-6.0 + 3.0 * resf)) / 6.0)
            if resf < 2.0  return ((2.0 - resf) * (2.0 - resf) * (2.0 - resf) / 6.0)
            return 0
        case Gaussian:
            if math.abs(resf) > 3.0 return 0
            return math.exp(-resf * resf * 0.5) / 0.79788456080287
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

    func calcContributions(lineSize, srcSize: int, scale: f64, rmode: Image.ResizeMethod)->LineContrib
    {
        filterWidth := getFilterWidth(rmode)

        var cscale, width: f64 = ?
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

        windowSize := 2 * (cast(int) math.ceil(width) + 1)

        var res: retval
        res = allocContributions(lineSize, windowSize)

        loop u: lineSize
        {
            center := u / scale

            left  := math.max(cast(int) math.floor(center - width), 0)
            right := math.min(cast(int) math.ceil(center + width), srcSize - 1)

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

    #[swag.safety("", false)]
    func scaleRow(imgDst, imgSrc: *Image, row: int, contrib: *LineContrib)
    {
        bpp8   := imgDst.bpp8
        dstRow := imgDst.pixels + row * bpp8 * imgDst.width
        srcRow := imgSrc.pixels + row * bpp8 * imgSrc.width

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            crow0 := contrib.row.buffer
            loop x: imgDst.width
            {
                crow  := crow0 + x
                left  := crow.left
                right := crow.right

                var da, dr, dg, db: f64
                pw  := crow.weights.buffer
                ptr := srcRow + left * bpp8
                loop (right - left) + 1
                {
                    f := :pw
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
        default: debug.assert(false)
        }
    }

    #[swag.safety("", false)]
    //#[swag.printbc]
    func scaleCol(imgDst, imgSrc: *Image, col: int, contrib: *LineContrib)
    {
        bpp8   := imgDst.bpp8
        dstCol := imgDst.pixels + col * bpp8
        srcCol := imgSrc.pixels + col * bpp8
        w8Dst  := imgDst.width * bpp8
        w8Src  := imgSrc.width * bpp8

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            crow0 := contrib.row.buffer
            loop y: imgDst.height
            {
                crow  := crow0 + y
                left  := crow.left
                right := crow.right

                var da, dr, dg, db: f64
                pw  := crow.weights.buffer
                ptr := srcCol + left * w8Src
                loop (right - left) + 1
                {
                    f := :pw
                    db += f * ptr[0]
                    dg += f * ptr[1]
                    dr += f * ptr[2]
                    #if BPP == 32 da += f * ptr[3]
                    pw += 1
                    ptr += w8Src
                }

                dstCol[0] = cast(u8) db
                dstCol[1] = cast(u8) dg
                dstCol[2] = cast(u8) dr
                #if BPP == 32 dstCol[3] = cast(u8) da
                dstCol += w8Dst
            }
        }

        switch imgSrc.bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: debug.assert(false)
        }
    }

    func scaleCol(imgDst, imgSrc: *Image, rmode: Image.ResizeMethod)
    {
        var t: time.Stopwatch
        t.start()

        sh := cast(int) imgSrc.height
        dh := cast(int) imgDst.height
        contrib := calcContributions(dh, sh, cast(f64) dh / sh, rmode)

        f := @{imgDst, imgSrc, contrib: &contrib}
        jobs.parallelFor(imgDst.width, &f)
        {
            params := cast(*@typeof(f)) data
            scaleCol(params.imgDst, params.imgSrc, cast(int) index, params.contrib)
        }

        t.stop()
        elapsed := t.elapsedMilliseconds()
        console.println("scaleCol: %ms", elapsed)
    }

    func scaleRow(imgDst, imgSrc: *Image, rmode: Image.ResizeMethod)
    {
        var t: time.Stopwatch
        t.start()

        sw := cast(int) imgSrc.width
        dw := cast(int) imgDst.width
        contrib := calcContributions(dw, sw, cast(f64) dw / sw, rmode)

        f := @{imgDst, imgSrc, contrib: &contrib}
        jobs.parallelFor(imgDst.height, &f)
        {
            params := cast(*@typeof(f)) data
            scaleRow(params.imgDst, params.imgSrc, cast(int) index, params.contrib)
        }

        t.stop()
        elapsed := t.elapsedMilliseconds()
        console.println("scaleRow: %ms", elapsed)
    }

    func doResizeFilter(srcImage, dstImage: *Image, method: Image.ResizeMethod)
    {
        tmp := image.create(dstImage.width, srcImage.height, srcImage.pf)
        scaleRow(&tmp, srcImage, method)
        scaleCol(dstImage, &tmp, method)
    }

    func doResizeRaw(srcImage, dstImage: *Image)
    {
        xscale := srcImage.width / cast(f32) dstImage.width
        yscale := srcImage.height / cast(f32) dstImage.height
        f := @{xscale, yscale, srcImage}

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            dstImage.visitPixels(&f)
            {
                params := cast(*@typeof(f)) userData
                fx := cast(u32) (x * params.xscale)
                fy := cast(u32) (y * params.yscale)
                pixSrc := params.srcImage.pixels + (fx * image.bpp8) + (fy * params.srcImage.width * image.bpp8)
                Image.copyPixel'BPP(pix, pixSrc)
            }
        }

        switch srcImage.bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: debug.assert(false)
        }
    }

    func doResize(srcImage, dstImage: *Image, method: Image.ResizeMethod)
    {
        switch method
        {
        case Raw:   doResizeRaw(srcImage, dstImage)
        default:    doResizeFilter(srcImage, dstImage, method)
        }
    }
}