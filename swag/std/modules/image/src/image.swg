#global public
using swag, core

struct Image
{
    allocator:      IAllocator
    pixels:         *u8
    width, height:  u32
    pf:             PixelFormat = ?
    size:           uint
    bpp:            u8  // Bits per pixel
    bpp8:           u8  // Bytes per pixel
}

// Creates a new image surface
func create(width, height: u32, pf: PixelFormat)->Image
{
    debug.assert(width > 0 && height > 0, "invalid dimensions")

    var result: retval
    result.width = width
    result.height = height
    result.pf = pf
    result.allocator = @getcontext().allocator
    result.bpp = pf.bpp()
    result.bpp8 = cast(u8) (result.bpp >> 3)
    result.size = cast(uint) (width * height * result.bpp8)
    result.pixels = memory.alloc(result.size, result.allocator)
    return result
}

impl Image
{
    #[inline]
    private func free(using self)
    {
        memory.free(pixels, size, allocator)
    }

    func opDrop(using self)
    {
        free()
    }

    func opPostCopy(using self)
    {
        if pixels
        {
            newPixels := memory.alloc(size, allocator)
            memory.copy(newPixels, pixels, size)
            pixels = newPixels
        }
    }

    // Release the content of the image
    func release(using self)
    {
        free()
        @init(self)
    }

    // Clear the content of the image
    func clear(using self)
    {
        memory.clear(pixels, size)
    }

    // Returns the pixel address at the given x,y coordinate
    #[swag.inline]
    func getPixelAddress(using self, x, y: u32)->*u8
    {
        iy := height - y - 1
        return pixels + (x + (iy * width)) * bpp8
    }
}