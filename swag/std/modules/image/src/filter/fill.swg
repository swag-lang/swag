using core, drawing

impl Image
{
    public enum FillFormat
    {
        VerticalSaturation
    }

    // Transform image to grayscale, with a given factor
    public func fill(using self, fillFormat: FillFormat, color0: Color)
    {
        if pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8
        {
            f := fillFormat
            visitPixels(&f)
            {
                f := :cast(*FillFormat) data
            }
        }
        else
        {
            debug.assert(false, "unsupported pixel format")
        }
    }
}