using core, drawing

impl Image
{
    // Fill with a gradient from 'color0' to 'color1'
    public func fillGradient2(using self, color0, color1: Color)
    {
        debug.assert(pf == .BGR8 || pf == .BGRA8)

        pix := pixels
        loop x: width
        {
            factor0 := x / cast(f32) width
            pix[0] = cast(u8) math.lerp(cast(f32) color0.b, cast(f32) color1.b, factor0)
            pix[1] = cast(u8) math.lerp(cast(f32) color0.g, cast(f32) color1.g, factor0)
            pix[2] = cast(u8) math.lerp(cast(f32) color0.r, cast(f32) color1.r, factor0)
            if pf == .BGRA8 pix[3] = cast(u8) math.lerp(cast(f32) color0.a, cast(f32) color1.a, factor0)
            pix += bpp8
        }

        visitPixels(stride: width)
        {
            memory.copy(pix, image.pixels, cast(uint) image.width * image.bpp8)
        }
    }

    // Fill with a gradient with a different color at each corner
    public func fillGradient4(using self, color0, color1, color2, color3: Color)
    {
        debug.assert(pf == .BGR8 || pf == .BGRA8)

        f := @{color0, color1, color2, color3}
        visitPixels(&f)
        {
            params := cast(*@typeof(f)) userData
            factor0 := x / cast(f32) image.width
            factor1 := y / cast(f32) image.height
            colH1 := Color.getBlend(params.color0, params.color1, factor0)
            colH0 := Color.getBlend(params.color2, params.color3, factor0)
            col := Color.getBlend(colH0, colH1, factor1)
            pix[0] = col.b; pix[1] = col.g; pix[2] = col.r
        }
    }
}