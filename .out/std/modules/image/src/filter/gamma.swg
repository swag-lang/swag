using core

impl Image
{
    // Change the gamma.
    public func gamma(using self, level: f32 = 2.2)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        g := 1.0 / Math.clamp(level, Math.ConstF32.Epsilon, 5)

        var convArr: [256] u8 = ?
        loop i: 256
            convArr[i] = cast(u8) (Math.pow(i / 255.0, g) * 255)

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