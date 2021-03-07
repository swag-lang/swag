using core, drawing

impl Image
{
    public enum FillFormat
    {
        SaturationVertical
        HueSaturation
    }

    public func fill(using self, fillFormat: FillFormat, color0: Color = Argb.White) throw
    {
        if pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8
        {
            hsl0 := color0.getHsl()
            f := @{fillFormat, col0: hsl0}
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData

                var col: Color = ?
                using FillFormat
                switch(params.item0)
                {
                case SaturationVertical:
                {
                    factor1 := y / cast(f32) image.height
                    col = Color.fromHsl(params.col0.h, cast(f32) factor1, 0.5)
                }

                case HueSaturation:
                {
                    factor0 := x / cast(f32) image.width
                    factor1 := y / cast(f32) image.height
                    col = Color.fromHsl(cast(f32) factor0, cast(f32) factor1, 0.5)
                }
                }

                pix[0] = col.b
                pix[1] = col.g
                pix[2] = col.r
            }
        }
        else
        {
            throw "unsupported pixel format"
        }
    }
}