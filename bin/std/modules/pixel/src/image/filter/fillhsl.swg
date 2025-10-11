using Core

impl Image
{
    public enum FillHslType
    {
        HueVert
        HueHorz
        SaturationVert
        SaturationHorz
        LightnessVert
        LightnessHorz
        HueSaturation
        HueLightness
        SaturationLightness     // (h,0,1) (h,1,0.5) (h,0,0) (h,1,0)
        GreenBlue
        RedBlue
        RedGreen
    }

    public mtd fillHsl(fillType: FillHslType, color: Color = Argb.White)
    {
        // unsupported pixel format
        if .pf.channels() < 3 or .pf.bitDepth() != 8 do
            return

        let hsl    = color.toHsl()
        var swapRB = .pf == .RGB8 or .pf == .RGBA8

        #[Swag.Complete]
        switch fillType
        {
        case HueVert:
            .visitPixels(&swapRB)
            {
                let swapRB  = dref cast(*bool) userData
                let factor0 = y / cast(f32) image.height
                let col     = Color.fromHsl(factor0, 1, 0.5)
                if swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }

        case HueHorz:
            var pix = .pixels.buffer
            for x in .width
            {
                let factor0 = x / cast(f32) .width
                let col     = Color.fromHsl(factor0, 1, 0.5)
                if swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if .pf.hasAlpha() do
                    pix[3] = 255
                pix += .bpp8
            }

            .visitPixels(stride: .width)
            {
                Memory.copy(pix, image.pixels.buffer, cast(u64) image.width * image.bpp8)
            }

        case SaturationVert:
            var pms = {hsl, swapRB}
            .visitPixels(&pms)
            {
                let params  = dref cast(*#decltype(pms)) userData
                let swapRB  = params.swapRB
                let factor0 = y / cast(f32) image.height
                let col     = Color.fromHsl(params.hsl.h, factor0, 0.5)
                if swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }

        case SaturationHorz:
            var pix = .pixels.buffer
            for x in .width
            {
                let factor0 = x / cast(f32) .width
                let col     = Color.fromHsl(hsl.h, factor0, 0.5)
                if swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if .pf.hasAlpha() do
                    pix[3] = 255
                pix += .bpp8
            }

            .visitPixels(stride: .width)
            {
                Memory.copy(pix, image.pixels.buffer, cast(u64) image.width * image.bpp8)
            }

        case LightnessVert:
            var pms = {hsl, swapRB}
            .visitPixels(&pms)
            {
                let params  = dref cast(*#decltype(pms)) userData
                let swapRB  = params.swapRB
                let factor0 = y / cast(f32) image.height
                let col     = Color.fromHsl(params.hsl.h, 1, factor0)
                if swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }

        case LightnessHorz:
            var pix = .pixels.buffer
            for x in .width
            {
                let factor0 = x / cast(f32) .width
                let col     = Color.fromHsl(hsl.h, hsl.s, factor0)
                if swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if .pf.hasAlpha() do
                    pix[3] = 255
                pix += .bpp8
            }

            .visitPixels(stride: .width)
            {
                Memory.copy(pix, image.pixels.buffer, cast(u64) image.width * image.bpp8)
            }

        case HueSaturation:
            var pms = {hsl, swapRB}
            .visitPixels(&pms)
            {
                let params  = dref cast(*#decltype(pms)) userData
                let factor0 = x / cast(f32) image.width
                let factor1 = y / cast(f32) image.height
                let col     = Color.fromHsl(factor0, factor1, 0.5)
                if params.swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }

        case HueLightness:
            var pms = {hsl, swapRB}
            .visitPixels(&pms)
            {
                let params  = dref cast(*#decltype(pms)) userData
                let factor0 = x / cast(f32) image.width
                let factor1 = y / cast(f32) image.height
                let col     = Color.fromHsl(factor0, 1, factor1)
                if params.swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }

        case SaturationLightness:
            var pms = {hsl, swapRB}
            .visitPixels(&pms)
            {
                let params  = dref cast(*#decltype(pms)) userData
                let factor0 = x / cast(f32) image.width
                let factor1 = y / cast(f32) image.height
                var l       = 0.5 + ((1 - factor0) * 0.5)
                l = Math.lerp(0, l, factor1)
                let col = Color.fromHsl(params.hsl.h, factor0, l)
                if params.swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }

        case GreenBlue:
            var pms = {color.r / 255.0, swapRB}
            .visitPixels(&pms)
            {
                let params  = dref cast(*#decltype(pms)) userData
                let factor0 = x / cast(f32) image.width
                let factor1 = 1 - (y / cast(f32) image.height)
                let col     = Color.fromRgbf(params.item0, factor0, factor1)
                if params.swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }

        case RedBlue:
            var pms = {color.g / 255.0, swapRB}
            .visitPixels(&pms)
            {
                let params  = dref cast(*#decltype(pms)) userData
                let factor0 = x / cast(f32) image.width
                let factor1 = 1 - (y / cast(f32) image.height)
                let col     = Color.fromRgbf(factor0, params.item0, factor1)
                if params.swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }

        case RedGreen:
            var pms = {color.b / 255.0, swapRB}
            .visitPixels(&pms)
            {
                let params  = dref cast(*#decltype(pms)) userData
                let factor0 = x / cast(f32) image.width
                let factor1 = 1 - (y / cast(f32) image.height)
                let col     = Color.fromRgbf(factor0, factor1, params.item0)
                if params.swapRB
                {
                    pix[0] = col.r
                    pix[1] = col.g
                    pix[2] = col.b
                }
                else
                {
                    pix[0] = col.b
                    pix[1] = col.g
                    pix[2] = col.r
                }
                if image.pf.hasAlpha() do
                    pix[3] = 255
            }
        }
    }
}
