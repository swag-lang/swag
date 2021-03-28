using core

impl Image
{
    private func doChangePf(using self, dstImage: *Image)
    {
        f := dstImage
        visitPixels(f)
        {
            dstImage := cast(*Image) userData
            dstPix = dstImage.pixels + x * dstImage.bpp8 + y * dstImage.width8
        }
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