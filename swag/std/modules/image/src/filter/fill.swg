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
                img := params.item1
                offset := (cast(uint) (buffer - img.pixels)) / img.bpp8
                x := offset % img.width
                y := offset / img.width

                factor := y / cast(f32) img.height

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