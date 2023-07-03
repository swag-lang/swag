using Core

impl Image
{
    // Fill with an horizontal gradient from 'color0' to 'color1'
    public mtd fillGradient2(color0, color1: Color)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        #[Swag.Mixin]
        func(PF: PixelFormat) doIt()
        {
            pix := pixels.buffer
            loop x: width
            {
                factor0 := x / cast(f32) width
                b := cast(u8) Math.lerp(cast(f32) color0.b, cast(f32) color1.b, factor0)
                g := cast(u8) Math.lerp(cast(f32) color0.g, cast(f32) color1.g, factor0)
                r := cast(u8) Math.lerp(cast(f32) color0.r, cast(f32) color1.r, factor0)
                a := 255'u8
                if PF.bpp() == 32
                    a = cast(u8) Math.lerp(cast(f32) color0.a, cast(f32) color1.a, factor0)
                setPixelRGBA8'PF(pix, r, g, b, a)
                pix += bpp8
            }
        }

        switch pf
        {
        case BGR8:  doIt'BGR8()
        case BGRA8: doIt'BGRA8()
        case RGB8:  doIt'RGB8()
        case RGBA8: doIt'RGBA8()
        default:    Debug.assert(false)
        }

        visitPixels(stride = width)
        {
            Memory.copy(pix, image.pixels.buffer, cast(u64) image.width * image.bpp8)
        }
    }

    // Fill with a gradient with a different color at each corner
    public mtd fillGradient4(topLeft, topRight, bottomLeft, bottomRight: Color)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        #[Swag.Mixin]
        func(PF: PixelFormat) doIt()
        {
            visitPixels(&f)
            {
                params := cast(*@decltype(f)) userData
                factor0 := x / cast(f32) image.width
                factor1 := y / cast(f32) image.height
                colH1 := Color.getBlend(params.topLeft, params.topRight, factor0)
                colH0 := Color.getBlend(params.bottomLeft, params.bottomRight, factor0)
                col := Color.getBlend(colH0, colH1, factor1)
                setPixelRGBA8'PF(pix, col.r, col.g, col.b, col.a)
            }
        }

        f := {topLeft, topRight, bottomLeft, bottomRight}
        switch pf
        {
        case BGR8:  doIt'BGR8()
        case BGRA8: doIt'BGRA8()
        case RGB8:  doIt'RGB8()
        case RGBA8: doIt'RGBA8()
        default:    Debug.assert(false)
        }
    }
}