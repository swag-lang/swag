using core

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
    case Bilinear: return 1
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
    }

    return 0
}

func calcContributions(lineSize, srcSize: int, _scale: f64, rmode: Image.ResizeMethod)->LineContrib
{
    filterWidth := getFilterWidth(rmode)

    var scale, width: f64 = ?
    if _scale < 1.0
    {
        width = filterWidth / _scale
        scale = _scale
    }
    else
    {
        width = filterWidth
        scale = 1.0
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
            resf := getFilter(scale * (src - center), rmode)
            if (resf > 0)
                resf *= scale
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
            for src := left; src <= right; src += 1
            {
                res.row[u].weights[src - left] /= totalWeight
            }
        }

        res.row[u].left  = left
        res.row[u].right = right
    }

    return res
}

func scaleRow(imgDst, imgSrc: *Image, row: int, contrib: LineContrib)
{
    bpp8   := imgDst.bpp8
    dstRow := imgDst.pixels + row * bpp8 * imgDst.width
    srcRow := imgSrc.pixels + row * bpp8 * imgSrc.width

    loop x: imgDst.width
    {
        left  := contrib.row[x].left
        right := contrib.row[x].right

        var da, dr, dg, db: f64
        pw := contrib.row[x].weights.buffer
        for i := left; i <= right; { i += 1; pw += 1; }
        {
            ptr := srcRow + i * bpp8
            db += :pw * ptr[0];
            dg += :pw * ptr[1];
            dr += :pw * ptr[2];
            da += :pw * ptr[3];
        }

        dstRow[0] = cast(u8) db
        dstRow[1] = cast(u8) dg
        dstRow[2] = cast(u8) dr
        dstRow[3] = cast(u8) da
        dstRow += bpp8
        srcRow += bpp8
    }
}

func scaleCol(imgDst, imgSrc: *Image, col: int, contrib: LineContrib)
{
    bpp8   := imgDst.bpp8
    dstCol := imgDst.pixels + col * bpp8
    srcCol := imgSrc.pixels + col * bpp8
    w8Dst  := imgDst.width * bpp8
    w8Src  := imgSrc.width * bpp8

    loop y: imgDst.height
    {
        left  := contrib.row[y].left
        right := contrib.row[y].right

        var da, dr, dg, db: f64
        pw := contrib.row[y].weights.buffer
        for i := left; i <= right; { i += 1; pw += 1; }
        {
            ptr := srcCol + i * w8Src
            db += :pw * ptr[0]
            dg += :pw * ptr[1]
            dr += :pw * ptr[2]
            da += :pw * ptr[3]
        }

        dstCol[0] = cast(u8) db
        dstCol[1] = cast(u8) dg
        dstCol[2] = cast(u8) dr
        dstCol[3] = cast(u8) da
        dstCol += w8Dst
        srcCol += w8Src
    }
}

func vertScale(imgDst, imgSrc: *Image, rmode: Image.ResizeMethod)
{
    sh := cast(int) imgSrc.height
    dh := cast(int) imgDst.height
    contrib := calcContributions(dh, sh, cast(f64) dh / sh, rmode)
    loop i: imgDst.width
        scaleCol(imgDst, imgSrc, cast(int) i, contrib)
}

func horizScale(imgDst, imgSrc: *Image, rmode: Image.ResizeMethod)
{
    sw := cast(int) imgSrc.width
    dw := cast(int) imgDst.width
    contrib := calcContributions(dw, sw, cast(f64) dw / sw, rmode)
    loop i: imgDst.height
        scaleRow(imgDst, imgSrc, cast(int) i, contrib)
}

impl Image
{
    public enum ResizeMethod
    {
        Raw
        Bilinear
    }

    private func doResizeSlow(using self, dstImage: *Image, method: ResizeMethod)
    {
        tmp := image.create(dstImage.width, height, pf)
        horizScale(&tmp, self, method)
        vertScale(dstImage, &tmp, method)
    }

    private func doResizeRaw(using self, dstImage: *Image)
    {
        xscale := width / cast(f32) dstImage.width
        yscale := height / cast(f32) dstImage.height
        f := @{xscale, yscale, self}

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            dstImage.visitPixels(&f)
            {
                params := cast(*@typeof(f)) userData
                fx := cast(u32) (x * params.xscale)
                fy := cast(u32) (y * params.yscale)
                pixSrc := params.self.pixels + (fx * image.bpp8) + (fy * params.self.width * image.bpp8)
                copyPixel'BPP(pix, pixSrc)
            }
        }

        switch bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: debug.assert(false)
        }
    }

    private func doResize(using self, dstImage: *Image, method: ResizeMethod)
    {
        switch method
        {
        case Raw:   doResizeRaw(dstImage)
        default:    doResizeSlow(dstImage, method)
        }
    }

    // Resize image
    public func resize(using self, newWidth, newHeight: u32, method = ResizeMethod.Raw)
    {
        debug.assert(newWidth > 0 && newHeight > 0)

        if newWidth == width && newHeight == height return

        allocWorkingBuffer(newWidth, newHeight, pf)
        doResize(workingBuffer, method)
        applyWorkingBuffer()
    }

    // Resize image
    public func toResize(using self, newWidth, newHeight: u32, method = ResizeMethod.Raw)->Image
    {
        debug.assert(newWidth > 0 && newHeight > 0)

        if newWidth == width && newHeight == height
        {
            var result: retval = :self
            return result
        }

        var result: retval = create(newWidth, newHeight, pf)
        doResize(&result, method)
        return result
    }
}