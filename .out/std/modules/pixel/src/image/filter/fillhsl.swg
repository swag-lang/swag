using Core, Drawing

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

    public func fillHsl(using self, fillType: FillHslType , color: Color = Argb.White)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        hsl := color.toHsl()
        swapRB := pf == .RGB8 or pf == .RGBA8

        #[Swag.Complete]
        switch fillType
        {
        case Hue:
            pix := pixels
            loop x: width
            {
                factor0 := x / cast(f32) width
                col := Color.fromHsl(factor0, hsl.s, 0.5)
                if swapRB { pix[0] = col.r; pix[1] = col.g; pix[2] = col.b; }
                else      { pix[0] = col.b; pix[1] = col.g; pix[2] = col.r; }
                if swapRB swap(pix, pix + 2)
                pix += bpp8
            }

            visitPixels(stride: width)
            {
                Memory.copy(pix, image.pixels, cast(uint) image.width * image.bpp8)
            }

        case Saturation:
            pix := pixels
            loop x: width
            {
                factor0 := x / cast(f32) width
                col := Color.fromHsl(hsl.h, factor0, 0.5)
                if swapRB { pix[0] = col.r; pix[1] = col.g; pix[2] = col.b; }
                else      { pix[0] = col.b; pix[1] = col.g; pix[2] = col.r; }
                pix += bpp8
            }

            visitPixels(stride: width)
            {
                Memory.copy(pix, image.pixels, cast(uint) image.width * image.bpp8)
            }

        case Lightness:
            pix := pixels
            loop x: width
            {
                factor0 := x / cast(f32) width
                col := Color.fromHsl(hsl.h, hsl.s, factor0)
                if swapRB { pix[0] = col.r; pix[1] = col.g; pix[2] = col.b; }
                else      { pix[0] = col.b; pix[1] = col.g; pix[2] = col.r; }
                pix += bpp8
            }

            visitPixels(stride: width)
            {
                Memory.copy(pix, image.pixels, cast(uint) image.width * image.bpp8)
            }

        case HueSaturation:
            visitPixels(&swapRB)
            {
                swapRB := dref cast(*bool) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(factor0, factor1, 0.5)
                if swapRB { pix[0] = col.r; pix[1] = col.g; pix[2] = col.b; }
                else      { pix[0] = col.b; pix[1] = col.g; pix[2] = col.r; }
            }

        case HueLightness:
            pms := @{hsl, swapRB}
            visitPixels(&pms)
            {
                params := dref cast(*@typeof(pms)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(factor0, params.hsl.s, factor1)
                if params.swapRB { pix[0] = col.r; pix[1] = col.g; pix[2] = col.b; }
                else             { pix[0] = col.b; pix[1] = col.g; pix[2] = col.r; }
            }

        case SaturationLightness:
            pms := @{hsl, swapRB}
            visitPixels(&pms)
            {
                params := dref cast(*@typeof(pms)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                col := Color.fromHsl(params.hsl.h, factor0, factor1)
                if params.swapRB { pix[0] = col.r; pix[1] = col.g; pix[2] = col.b; }
                else             { pix[0] = col.b; pix[1] = col.g; pix[2] = col.r; }
            }
        }
    }
}