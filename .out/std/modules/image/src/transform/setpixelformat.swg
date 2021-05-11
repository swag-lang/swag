using core

impl Image
{
    private func doChangePf(using self, dstImage: *Image)
    {
        using PixelFormat

        f := dstImage

        #[swag.complete]
        switch dstImage.pf
        {
        case RGB8:
            break
        case RGBA8:
            break

        case BGR8:
            if pf == BGRA8
            {
                visitPixels(f)
                {
                    dstImage := cast(*Image) userData
                    pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                    :cast(*u16) pixDst = :cast(*u16) pix
                    pixDst[2] = pix[2]
                }
                return
            }

        case BGRA8:
            if pf == BGR8
            {
                visitPixels(f)
                {
                    dstImage := cast(*Image) userData
                    pixDst := dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
                    :cast(*u16) pixDst = :cast(*u16) pix
                    pixDst[2] = pix[2]
                    pixDst[3] = 255
                }
                return
            }
        }

        debug.assert(false)
    }

    // Change image pixel format
    public func setPixelFormat(using self, newPf: PixelFormat)
    {
        debug.assert(pixels != null)

        if newPf == pf return
        allocWorkingBuffer(width, height, newPf)
        doChangePf(workingBuffer)
        applyWorkingBuffer()
    }

    // Change image pixel format
    public func toSetPixelFormat(using self, newPf: PixelFormat)->Image
    {
        debug.assert(pixels != null)

        if newPf == pf
        {
            var result: retval = :self
            return result
        }

        var result: retval = create(width, height, newPf)
        doChangePf(&result)
        return result
    }
}