using core, drawing

impl Image
{
    public enum FillGradientType
    {
        Horizontal
    }

    public func fillGradient(using self, fillType: FillGradientType, color0, color1: Color) throw
    {
        if pf != PixelFormat.BGR8 && pf != PixelFormat.BGRA8
            throw "unsupported pixel format"

        using FillGradientType
        switch(fillType)
        {
        case Horizontal:
            // Lerp only for the first line
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

            // Then copy that first line to the rest of the image
            visitPixels(null, width)
            {
                @memcpy(pix, image.pixels, cast(uint) image.width * image.bpp8)
            }
        }
    }
}