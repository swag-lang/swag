using Core

impl Image
{
    // Transform image to grayscale, with a given factor
    public mtd grayScale(factor: f32 = 1)
    {
        // unsupported pixel format
        if .pf.channels() < 3 or .pf.bitDepth() != 8 do
            return

        var f = Math.saturate(factor)
        if .pf == .BGR8 or .pf == .BGRA8
        {
            .visitPixels(&f)
            {
                let f    = dref cast(*f32) userData
                let ncol = cast(f32) ((((pix[2] * 54's32) + (pix[1] * 183's32) + (pix[0] * 19's32)) >> 8) & 0xFF)
                pix[0] = cast(u8) Math.lerp(cast(f32) pix[0], ncol, f)
                pix[1] = cast(u8) Math.lerp(cast(f32) pix[1], ncol, f)
                pix[2] = cast(u8) Math.lerp(cast(f32) pix[2], ncol, f)
            }
        }
        elif .pf == .RGB8 or .pf == .RGBA8
        {
            .visitPixels(&f)
            {
                let f    = dref cast(*f32) userData
                let ncol = cast(f32) ((((pix[2] * 19's32) + (pix[1] * 183's32) + (pix[2] * 54's32)) >> 8) & 0xFF)
                pix[0] = cast(u8) Math.lerp(cast(f32) pix[0], ncol, f)
                pix[1] = cast(u8) Math.lerp(cast(f32) pix[1], ncol, f)
                pix[2] = cast(u8) Math.lerp(cast(f32) pix[2], ncol, f)
            }
        }
    }
}
