using core, drawing

impl Image
{
    // Fill image with 'color'
    public func fill(using self, color: Color)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        #[Swag.mixin]
        func(PF: PixelFormat) fillFirstLine()
        {
            pix := pixels
            loop x: width
            {
                setPixelRGBA8'PF(pix, color.r, color.g, color.b, color.a)
                pix += bpp8
            }
        }

        switch pf
        {
        case BGR8:  fillFirstLine'BGR8()
        case BGRA8: fillFirstLine'BGRA8()
        case RGB8:  fillFirstLine'RGB8()
        case RGBA8: fillFirstLine'RGBA8()
        default:    debug.assert(false)
        }

        visitPixels(stride: width)
        {
            Memory.copy(pix, image.pixels, cast(uint) image.width * image.bpp8)
        }
    }
}