using core

impl Image
{
    // Transform image to grayscale, with a given factor
    public func lightness(using self, factor: f32 = 1)
    {
        debug.assert(pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8)

        f := @{math.saturate(factor)}
        visitPixels(&f)
        {
            f := :cast(*@typeof(f)) userData
            /*ncol := cast(f32) ((((pix[2] * 54) + (pix[1] * 183) + (pix[0] * 19)) >>% 8) & 0xFF)
            pix[0] = cast(u8) math.lerp(cast(f32) pix[0], ncol, f)
            pix[1] = cast(u8) math.lerp(cast(f32) pix[1], ncol, f)
            pix[2] = cast(u8) math.lerp(cast(f32) pix[2], ncol, f)*/
        }
    }
}