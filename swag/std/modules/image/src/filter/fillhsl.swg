using core, drawing

impl Image
{
    public enum FillHslType
    {
        SaturationHorizontal
        SaturationVertical
        HueHorizontal
        HueVertical
        LightnessHorizontal
        LightnessVertical
        HueSaturation
        HueLightness
        SaturationLightness
    }

    public func fillHsl(using self, fillType: FillHslType , color: Color = Argb.White) throw
    {
        if pf != PixelFormat.BGR8 && pf != PixelFormat.BGRA8
            throw "unsupported pixel format"

        hsl := color.getHsl()
        f := @{col: hsl}

        using FillHslType
        switch(fillType)
        {
        case SaturationHorizontal:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor0 := x / cast(f32) image.width
                col := Color.fromHsl(params.col.h, factor0, params.col.l)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case SaturationVertical:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(params.col.h, factor1, params.col.l)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case HueHorizontal:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor0 := x / cast(f32) image.width
                col := Color.fromHsl(factor0, params.col.s, 0.5)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case HueVertical:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(factor1, params.col.s, 0.5)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case LightnessHorizontal:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor0 := x / cast(f32) image.width
                col := Color.fromHsl(params.col.h, params.col.s, factor0)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case LightnessVertical:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(params.col.h, params.col.s, factor1)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case HueSaturation:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(factor0, factor1, 0.5)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case HueLightness:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(factor0, params.col.s, factor1)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }

        case SaturationLightness:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(params.col.h, factor0, factor1)
                pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
            }
        }
    }
}