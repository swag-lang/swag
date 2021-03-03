using core

impl Image
{
    // Transform image to grayscale, with a given factor
    public func grayScale(using self, factor: f32 = 1)
    {
        f := math.saturate(factor)
        if pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8
        {
            visitPixels(&f)
            {
                f := :cast(*f32) data
                ncol := cast(f32) ((((buffer[2] * 54) + (buffer[1] * 183) + (buffer[0] * 19)) >>% 8) & 0xFF)
                buffer[0] = cast(u8) math.lerp(cast(f32) buffer[0], ncol, f)
                buffer[1] = cast(u8) math.lerp(cast(f32) buffer[1], ncol, f)
                buffer[2] = cast(u8) math.lerp(cast(f32) buffer[2], ncol, f)
            }
        }
        else
        {
            debug.assert(false, "unsupported pixel format")
        }
    }
}