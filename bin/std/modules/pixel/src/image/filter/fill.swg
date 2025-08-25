using Core

impl Image
{
    // Fill image with 'color'
    public mtd fill(color: Color)
    {
        // unsupported pixel format
        if me.pf.channels() < 3 or me.pf.bitDepth() != 8:
            return

        #[Swag.Mixin]
        func(PF: PixelFormat) fillFirstLine()
        {
            var pix = pixels.buffer
            for x in width
            {
                convertToRGBA8'PF(pix, color.r, color.g, color.b, color.a)
                pix += bpp8
            }
        }

        switch me.pf
        {
        case BGR8:
            fillFirstLine'BGR8()
        case BGRA8:
            fillFirstLine'BGRA8()
        case RGB8:
            fillFirstLine'RGB8()
        case RGBA8:
            fillFirstLine'RGBA8()
        default:
            Debug.assert(false)
        }

        me.visitPixels(stride: me.width)
        {
            Memory.copy(pix, image.pixels.buffer, cast(u64) image.width * image.bpp8)
        }
    }
}
