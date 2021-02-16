#global public
using swag, core

struct Image
{
    allocator:      IAllocator
    pixels:         *u8
    width, height:  u32
    pf:             PixelFormat = ?
    bpp:            u32
    size:           uint
}

// Creates a new image surface
func create(width, height: u32, pf: PixelFormat)->Image
{
    debug.assert(width > 0 && height > 0, "invalid dimensions")

    var result: retval
    result.width = width
    result.height = height
    result.allocator = @getcontext().allocator
    result.bpp = pf.bpp()
    result.size = cast(uint) (width * height * (result.bpp / 8))
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
}