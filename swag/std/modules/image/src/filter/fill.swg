using core, drawing

impl Image
{
    public enum FillFormat
    {
        VerticalSaturation
    }

    public func fill(using self, fillFormat: FillFormat, color0: Color = Argb.White) throw
    {
        if pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8
        {
            hsl0 := color0.getHSL()
            f := @{fillFormat, hsl0}
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor := y / cast(f32) image.height

                var col: Color = ?
                col.setHSL(params.item1.h, params.item1.s, cast(f32) factor)

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