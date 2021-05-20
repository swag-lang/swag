using core

impl Image
{
    private func toRGB8(using self, dstImage: *Image)
    {
        f := dstImage

        #[Swag.complete]
        switch pf
        {
        case RGB8:
            break
        case RGBA8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[0]
                pixDst[1] = pix[1]
                pixDst[2] = pix[2]
            }
        case BGR8, BGRA8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
            }
        }
    }

    private func toRGBA8(using self, dstImage: *Image)
    {
        f := dstImage

        #[Swag.complete]
        switch pf
        {
        case RGBA8:
            break
        case BGRA8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
                pixDst[3] = pix[3]
            }
        case RGB8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[0]
                pixDst[1] = pix[1]
                pixDst[2] = pix[2]
                pixDst[3] = 255
            }
        case BGR8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
                pixDst[3] = 255
            }
        }
    }

    private func toBGR8(using self, dstImage: *Image)
    {
        f := dstImage

        #[Swag.complete]
        switch pf
        {
        case BGR8:
            break
        case BGRA8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[0]
                pixDst[1] = pix[1]
                pixDst[2] = pix[2]
            }
        case RGB8, RGBA8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
            }
        }
    }

    private func toBGRA8(using self, dstImage: *Image)
    {
        f := dstImage

        #[Swag.complete]
        switch pf
        {
        case BGRA8:
            break
        case BGR8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[0]
                pixDst[1] = pix[1]
                pixDst[2] = pix[2]
                pixDst[3] = 255
            }
        case RGB8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
                pixDst[3] = 255
            }
        case RGBA8:
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                pixDst[0] = pix[2]
                pixDst[1] = pix[1]
                pixDst[2] = pix[0]
                pixDst[3] = pix[3]
            }
        }
    }

    private func doChangePf(using self, dstImage: *Image)
    {
        #[Swag.complete]
        switch dstImage.pf
        {
        case RGB8:  toRGB8(dstImage)
        case RGBA8: toRGBA8(dstImage)
        case BGR8:  toBGR8(dstImage)
        case BGRA8: toBGRA8(dstImage)
        }
    }

    // Change image pixel format
    public func setPixelFormat(using self, newPf: PixelFormat)
    {
        Debug.assert(pixels != null)

        if newPf == pf return
        allocWorkingBuffer(width, height, newPf)
        doChangePf(workingBuffer)
        applyWorkingBuffer()
    }

    // Change image pixel format
    public func toSetPixelFormat(using self, newPf: PixelFormat)->Image
    {
        Debug.assert(pixels != null)

        if newPf == pf
        {
            var result: retval = dref self
            return result
        }

        var result: retval = create(width, height, newPf)
        doChangePf(&result)
        return result
    }
}