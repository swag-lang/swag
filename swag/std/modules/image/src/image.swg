#global public
using swag, core

struct Image
{
    allocator:      IAllocator
    pixels:         *u8
    width, height:  uint
    pf:             PixelFormat = ?
    bpp:            u32
}

// Creates a new image surface
func create(width, height: uint, pf: PixelFormat)->Image
{
    debug.assert(width > 0 && height > 0, "invalid dimensions")

    var result: retval
    result.width = width
    result.height = height
    result.allocator = @getcontext().allocator
    result.bpp = pf.bpp()
    result.pixels = memory.alloc(result.width * result.height * (result.bpp / 8), result.allocator)
    return result
}