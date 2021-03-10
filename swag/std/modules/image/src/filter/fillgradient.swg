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

        console.print("% %", color0.r, color1.r)

        f := @{col0: color0, col1: color1}

        using FillGradientType
        switch(fillType)
        {
        case Horizontal:
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor0 := x / cast(f32) image.width
                pix[0] = cast(u8) math.lerp(cast(f32) params.col0.b, cast(f32) params.col1.b, factor0)
                pix[1] = cast(u8) math.lerp(cast(f32) params.col0.g, cast(f32) params.col1.g, factor0)
                pix[2] = cast(u8) math.lerp(cast(f32) params.col0.r, cast(f32) params.col1.r, factor0)
                pix[3] = cast(u8) math.lerp(cast(f32) params.col0.a, cast(f32) params.col1.a, factor0)
            }
        }
    }
}