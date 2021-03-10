using core, drawing

impl Image
{
    public enum FillHslType
    {
        Saturation
        Hue
        Lightness
        HueSaturation
        HueLightness
        SaturationLightness
    }

    public func fillHsl(using self, fillType: FillHslType , color: Color = Argb.White) throw
    {
        if pf != PixelFormat.BGR8 && pf != PixelFormat.BGRA8
            throw "unsupported pixel format"

        hsl := color.getHsl()

        using FillHslType
        switch fillType
        {
        case Hue:
            pix := pixels
            loop x: width
            {
                factor0 := x / cast(f32) width
                col := Color.fromHsl(factor0, hsl.s, 0.5)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
                pix += bpp8
            }

            visitPixels(stride: width)
            {
                @memcpy(pix, image.pixels, cast(uint) image.width * image.bpp8)
            }

        case Saturation:
            pix := pixels
            loop x: width
            {
                factor0 := x / cast(f32) width
                col := Color.fromHsl(hsl.h, factor0, 0.5)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
                pix += bpp8
            }

            visitPixels(stride: width)
            {
                @memcpy(pix, image.pixels, cast(uint) image.width * image.bpp8)
            }

        case Lightness:
            pix := pixels
            loop x: width
            {
                factor0 := x / cast(f32) width
                col := Color.fromHsl(hsl.h, hsl.s, factor0)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
                pix += bpp8
            }

            visitPixels(stride: width)
            {
                @memcpy(pix, image.pixels, cast(uint) image.width * image.bpp8)
            }

        case HueSaturation:
            visitPixels(&hsl)
            {
                params := :cast(*@typeof(hsl)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(factor0, factor1, 0.5)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case HueLightness:
            visitPixels(&hsl)
            {
                params := :cast(*@typeof(hsl)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(factor0, params.s, factor1)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case SaturationLightness:
            visitPixels(&hsl)
            {
                params := :cast(*@typeof(hsl)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(params.h, factor0, factor1)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }
        }
    }
}