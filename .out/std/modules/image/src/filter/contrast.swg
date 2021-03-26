using core

impl Image
{
    // Change the contrast. 'factor' is [-1, 1]
    public func contrast(using self, factor: f32 = 0.5)
    {
        debug.assert(pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8)

        level := math.clamp(factor, -1, 1) + 1.0
        var convArr: [256] u8 = ?
        loop i: 256 convArr[i] = cast(u8) math.clamp(((i - 128) * level) + 128, 0, 255)

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