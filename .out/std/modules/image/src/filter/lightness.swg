using core

impl Image
{
    // Transform image to grayscale, with a given factor
    public func lightness(using self, factor: f32 = 0.5)
    {
        debug.assert(pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8)

        level := cast(s32) (math.saturate(factor) * 255)
        var convArr: [256] u8 = ?
        loop i: 256 convArr[i] = cast(u8) math.clamp(i + level, 0, 255)

        f := @{&convArr[0]}
        visitPixels(&f)
        {
            f := :cast(*@typeof(f)) userData
            pix[0] = f.item0[pix[0]]
            pix[1] = f.item0[pix[1]]
            pix[2] = f.item0[pix[2]]
        }
    }
}