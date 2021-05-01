using core

impl Image
{
    // Change the lightness. 'factor' is [-1, 1]
    public func lightness(using self, factor: f32 = 0.5)
    {
        debug.assert(pf == .BGR8 or pf == .BGRA8)

        level := cast(s32) (math.clamp(factor, -1, 1) * 255)
        var convArr: [256] u8 = ?
        loop i: 256 convArr[i] = cast(u8) math.clamp(i + level, 0, 255)

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