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
            if pf == PixelFormat.BGRA8
            {
                visitPixels()
                {
                    x8 := x * image.bpp8
                    pixSrc := cast(*u32) (image.pixels + x8)
                    pixDst := cast(*u32) pix
                    :pixDst = :pixSrc
                }
            }
            else
            {
                visitPixels()
                {
                    x8 := x * image.bpp8
                    pix[0] = image.pixels[x8]
                    pix[1] = image.pixels[x8 + 1]
                    pix[2] = image.pixels[x8 + 2]
                }
            }
        }
    }
}