using core, drawing

impl Image
{
    public enum FillFormat
    {
        VerticalSaturation
    }

    public func fill(using self, fillFormat: FillFormat) throw//, color0: Color)
    {
        if pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8
        {
            f := @{fillFormat}
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor := y / cast(f32) image.height
                pix[0] = cast(u8) (factor * 255)
                pix[1] = pix[0]
                pix[2] = pix[0]
            }
        }
        else
        {
            throw "unsupported pixel format"
        }
    }
}