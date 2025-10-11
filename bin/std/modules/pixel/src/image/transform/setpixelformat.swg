using Core

impl Image
{
    mtd const doRGB8(dstImage: *Image)
    {
        let mec = cast #unconst (*Image) me

        #[Swag.Complete]
        switch .pf
        {
        case RGB8:
            break
        case RGBA8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[0]
                pixDst[1] = pix[1]
                pixDst[2] = pix[2]
            }
        case BGR8, BGRA8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
            }
        }
    }

    mtd const doRGBA8(dstImage: *Image)
    {
        let mec = cast #unconst (*Image) me

        #[Swag.Complete]
        switch .pf
        {
        case RGBA8:
            break
        case BGRA8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
                pixDst[3] = pix[3]
            }
        case RGB8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[0]
                pixDst[1] = pix[1]
                pixDst[2] = pix[2]
                pixDst[3] = 255
            }
        case BGR8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
                pixDst[3] = 255
            }
        }
    }

    mtd const doBGR8(dstImage: *Image)
    {
        let mec = cast #unconst (*Image) me

        #[Swag.Complete]
        switch .pf
        {
        case BGR8:
            break
        case BGRA8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[0]
                pixDst[1] = pix[1]
                pixDst[2] = pix[2]
            }
        case RGB8, RGBA8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
            }
        }
    }

    mtd const doBGRA8(dstImage: *Image)
    {
        let mec = cast #unconst (*Image) me

        #[Swag.Complete]
        switch .pf
        {
        case BGRA8:
            break
        case BGR8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[0]
                pixDst[1] = pix[1]
                pixDst[2] = pix[2]
                pixDst[3] = 255
            }
        case RGB8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
                pixDst[3] = 255
            }
        case RGBA8:
            mec.visitPixels(dstImage)
            {
                let dstImage = cast(*Image) userData
                let pixDst   = dstImage.pixels.buffer + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
                pixDst[3] = pix[3]
            }
        }
    }

    mtd const doChangePf(dstImage: *Image)
    {
        #[Swag.Complete]
        switch dstImage.pf
        {
        case RGB8:
            .doRGB8(dstImage)
        case RGBA8:
            .doRGBA8(dstImage)
        case BGR8:
            .doBGR8(dstImage)
        case BGRA8:
            .doBGRA8(dstImage)
        }
    }

    // Change image pixel format
    public mtd setPixelFormat(newPf: PixelFormat)
    {
        Debug.assert(.pixels.buffer != null)

        if newPf == .pf do
            return

        .allocWorkingBuffer(.width, .height, newPf)
        .doChangePf(.workingBuffer)
        .applyWorkingBuffer()
    }

    // Change image pixel format
    public mtd const toSetPixelFormat(newPf: PixelFormat)->Image
    {
        Debug.assert(.pixels.buffer != null)

        if newPf == .pf do
            return dref me

        var result: retval = create(.width, .height, newPf)
        .doChangePf(&result)
        return result
    }
}
