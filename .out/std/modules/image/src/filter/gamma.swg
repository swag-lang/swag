using core

impl Image
{
    // Change the gamma.
    public func gamma(using self, level: f32 = 2.2)
    {
        if pf != .BGR8 and pf != .BGRA8 return // unsupported

        g := 1.0 / math.clamp(level, math.ConstF32.Epsilon, 5)

        var convArr: [256] u8 = ?
        loop i: 256
            convArr[i] = cast(u8) (math.pow(i / 255.0, g) * 255)

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