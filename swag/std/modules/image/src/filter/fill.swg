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
            f := @{fillFormat, self}
            visitPixels(&f)
            {
                params := :cast(*@typeof(f)) data
                offset := (cast(uint) (buffer - params.item1.pixels)) / params.item1.bpp8
                x := offset % params.item1.width
                y := offset / params.item1.height

                factor := x / cast(f32) params.item1.width

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