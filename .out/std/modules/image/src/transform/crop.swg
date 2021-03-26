using core

impl Image
{
    private func doCrop(using self, dstImage: *Image)
    {
        dstImage.visitPixels()
        {
        }
    }

    // Divide image size by 2 with a bilinear 2x2 filter
    public func crop(using self)
    {
        allocWorkingBuffer(width / 2, height / 2, pf)
        doCrop(workingBuffer)
        applyWorkingBuffer()
    }

    // Divide image size by 2 with a bilinear 2x2 filter
    public func toCrop(using self)->Image
    {
        debug.assert(width > 1 && height > 1)
        var result: retval = create(width / 2, height / 2, pf)
        doCrop(&result)
        return result
    }
}