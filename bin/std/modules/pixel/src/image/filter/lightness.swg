using Core

impl Image
{
    // Change the lightness. 'factor' is [-1, 1]
    public func lightness(using self, factor: f32 = 0.5)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        level := cast(s32) (Math.clamp(factor, -1, 1) * 255)
        var convArr: [256] u8 = ?
        loop i: 256 convArr[i] = cast(u8) Math.clamp(i + level, 0, 255)

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