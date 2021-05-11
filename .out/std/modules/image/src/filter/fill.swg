using core, drawing

impl Image
{
    // Fill image with 'color'
    public func fill(using self, color: Color)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        pix := pixels
        if pf == .BGR8 or pf == .BGRA8
        {
            loop x: width
            {
                pix[0] = color.b
                pix[1] = color.g
                pix[2] = color.r
                if pf == .BGRA8 pix[3] = color.a
                pix += bpp8
            }
        }
        else if pf == .RGB8 or pf == .RGBA8
        {
            loop x: width
            {
                pix[0] = color.r
                pix[1] = color.g
                pix[2] = color.b
                if pf == .BGRA8 pix[3] = color.a
                pix += bpp8
            }
        }

        visitPixels(stride: width)
        {
            memory.copy(pix, image.pixels, cast(uint) image.width * image.bpp8)
        }
    }
}