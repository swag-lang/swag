#global public
using swag, core

struct Image
{
    allocator:          IAllocator
    pixels:             *u8
    workingBuffer:      *u8
    size:               uint
    width, height:      u32
    wbWidth, wbHeight:  u32
    pf:                 PixelFormat = ?
    bpp:                u8  // Bits per pixel
    bpp8:               u8  // Bytes per pixel
    wbBpp8:             u8
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
        memory.free(workingBuffer, size, allocator)
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

    #[swag.macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        scan := pixels
        loop width * height
        {
			#macro
			{
                @alias0 := `scan
				@alias1 := @index
				#mixin `stmt
			}

            scan += bpp8
        }
    }

    // Allocate a computing buffer with current image size
    func allocWorkingBuffer(using self)
    {
        if workingBuffer && wbWidth == width && wbHeight == height && wbBpp8 == bpp8
            return

        freeWorkingBuffer()
        workingBuffer = memory.alloc(size, allocator)
        wbWidth = width
        wbHeight = height
        wbBpp8 = bpp8
    }

    // Allocate a computing buffer with new sizes
    func allocWorkingBuffer(using self, newWidth, newHeight: u32, newBpp8: u8)
    {
        if workingBuffer && newWidth == wbWidth && newHeight == wbHeight && newBpp8 == wbBpp8
            return

        freeWorkingBuffer()
        wbWidth = newWidth
        wbHeight = newHeight
        wbBpp8 = newBpp8
        workingBuffer = memory.alloc(cast(uint) wbWidth * wbHeight * wbBpp8, allocator)
    }

    // Free the working buffer if allocated
    func freeWorkingBuffer(using self)
    {
        memory.free(workingBuffer, cast(uint) wbWidth * wbHeight * wbBpp8)
        workingBuffer = null
    }

    // Replace the pixel buffer with the working buffer
    func applyWorkingBuffer(using self)
    {
        debug.assert(workingBuffer != null)
        swap(&pixels, &workingBuffer)
        swap(&wbWidth, &width)
        swap(&wbHeight, &height)
        swap(&wbBpp8, &bpp8)
        size = width * height * bpp8
    }

    // Macro to visit all pixels of the image in parallel chunks
    // Exported variables:
    // - pix: address of the pixel
    // - userData
    // - image: the processed image
    // - index: the pixel index
    // - x, y: the pixel coordinates
    #[swag.macro]
    func visitPixels(using self, userData: *void = null, stride: u32 = 1, stmt: code)
    {
        debug.assert(stride > 0)

        p := @{self, userData}
        jobs.parallelVisit(@mkslice(pixels, size), cast(u32) bpp8 * stride, &p)
        {
            p := cast(*@typeof(p)) data
            #macro
            {
                image := `p.item0
                userData := `p.item1
                pix := `buffer

                index := (cast(uint) (pix - image.pixels)) / image.bpp8
                x := cast(u32) index % image.width
                y := cast(u32) index / image.width

                #mixin `stmt
            }
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

    // Copy one pixel, depending on bpp
    #[swag.inline]
    func(BPP: u8) copyPixel(pixDst, pixSrc: *u8)
    {
        #if BPP == 24
        {
            :cast(*u16) pixDst = :cast(*u16) pixSrc
            pixDst[2] = pixSrc[2]
        }
        #elif BPP == 32
        {
            :cast(*u32) pixDst = :cast(*u32) pixSrc
        }
        #else
        {
            #assert "invalid pixelCopy BPP"
        }
    }
}