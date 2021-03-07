using core, drawing

impl Image
{
    public enum FillFormat
    {
        VerticalSaturation
    }

    // Transform image to grayscale, with a given factor
    //#[swag.printbc]
    public func fill(using self, fillFormat: FillFormat)//, color0: Color)
    {
        if pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8
        {
            f := @{fillFormat}
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) userData
                factor := y / cast(f32) image.height
                buffer[0] = cast(u8) (factor * 255)
                buffer[1] = buffer[0]
                buffer[2] = buffer[0]
            }
        }
        else
        {
            debug.assert(false, "unsupported pixel format")
        }
    }
}