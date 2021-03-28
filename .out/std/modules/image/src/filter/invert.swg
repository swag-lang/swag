using core

impl Image
{
    // Invert colors
    public func invert(using self)
    {
        debug.assert(pf == .BGR8 || pf == .BGRA8)

        var convArr: [256] u8 = ?
        loop i: 256 convArr[i] = cast(u8) (255 - i)

        f := &convArr[0]
        visitPixels(f)
        {
            arr := cast(*u8) userData
            pix[0] = arr[pix[0]]
            pix[1] = arr[pix[1]]
            pix[2] = arr[pix[2]]
        }
    }
}