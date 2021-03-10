using core, drawing

impl Image
{
    public enum FillGradientType
    {
        Color2
    }

    public func fillGradient(using self, fillType: FillGradientType, color0, color1: Color)
    {
        debug.assert(pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8)

        using FillGradientType
        switch fillType
        {
        case Color2:
            pix := pixels
            loop x: width
            {
                factor0 := x / cast(f32) width
                pix[0] = cast(u8) math.lerp(cast(f32) color0.b, cast(f32) color1.b, factor0)
                pix[1] = cast(u8) math.lerp(cast(f32) color0.g, cast(f32) color1.g, factor0)
                pix[2] = cast(u8) math.lerp(cast(f32) color0.r, cast(f32) color1.r, factor0)
                if pf == PixelFormat.BGRA8 pix[3] = cast(u8) math.lerp(cast(f32) color0.a, cast(f32) color1.a, factor0)
                pix += bpp8
            }

            visitPixels(stride: width)
            {
                @memcpy(pix, image.pixels, cast(uint) image.width * image.bpp8)
            }
        }
    }
}