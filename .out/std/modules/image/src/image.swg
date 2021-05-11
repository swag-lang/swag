#global public
using swag, core

struct Image
{
    allocator:          IAllocator
    pixels:             *u8
    workingBuffer:      *Image
    size:               uint
    width, height:      s32
    width8:             s32
    pf:                 PixelFormat = ?
    bpp:                u8  // Bits per pixel
    bpp8:               u8  // Bytes per pixel
}

// Creates a new image surface
func create(width, height: s32, pf: PixelFormat)->Image
{
    debug.assert(width > 0 and height > 0, "invalid dimensions")

    var result: retval
    result.width = width
    result.height = height
    result.pf = pf
    result.allocator = @getcontext().allocator
    result.bpp = pf.bpp()
    result.bpp8 = cast(u8) (result.bpp >> 3)
    result.width8 = width * result.bpp8
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
        memory.delete'Image(workingBuffer, allocator)
    }

    func opDrop(using self)
    {
        free()
    }

    func opPostCopy(using self)
    {
        workingBuffer = null
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
        allocWorkingBuffer(width, height, pf)
    }

    // Allocate a computing buffer with new sizes
    func allocWorkingBuffer(using self, newWidth, newHeight: s32, newPf: PixelFormat)
    {
        if  workingBuffer and
            newWidth == workingBuffer.width and
            newHeight == workingBuffer.height and
            newPf == workingBuffer.pf
            return

        freeWorkingBuffer()
        workingBuffer = memory.new'Image(allocator)
        :workingBuffer = image.create(newWidth, newHeight, newPf)
    }

    // Free the working buffer if allocated
    func freeWorkingBuffer(using self)
    {
        memory.delete(workingBuffer, allocator)
        workingBuffer = null
    }

    // Replace the current image content with the working buffer
    func applyWorkingBuffer(using self)
    {
        debug.assert(workingBuffer != null)
        swap(&pixels, &workingBuffer.pixels)
        swap(&width, &workingBuffer.width)
        swap(&height, &workingBuffer.height)
        swap(&bpp, &workingBuffer.bpp)
        swap(&bpp8, &workingBuffer.bpp8)
        swap(&size, &workingBuffer.size)
        swap(&width8, &workingBuffer.width8)
    }

    // Macro to visit all pixels of the image in parallel chunks
    // Exported variables:
    // - pix: address of the pixel
    // - userData
    // - image: the processed image
    // - index: the pixel index
    // - x, y: the pixel coordinates
    #[swag.macro]
    func visitPixels(using self, userData: *void = null, stride: s32 = 1, stmt: code)
    {
        debug.assert(stride > 0)

        p := @{self, userData}
        jobs.parallelVisit(@mkslice(pixels, size), cast(u32) bpp8 * stride, &p)
        {
            p := cast(*@typeof(p)) data
            #macro
            {
                image := `p.self
                userData := `p.userData
                pix := `buffer

                index := (cast(uint) (pix - image.pixels)) / image.bpp8
                x := cast(s32) index % image.width
                y := cast(s32) index / image.width

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

    // Set pixel values depending on pixel format
    func(PF: PixelFormat) setPixelRGBA8(pixDst: *u8, r, g, b, a: u8)
    {
        #if PF == .BGR8  { pixDst[0] = b; pixDst[1] = g; pixDst[2] = r; }
        #if PF == .BGRA8 { pixDst[0] = b; pixDst[1] = g; pixDst[2] = r; pixDst[3] = a; }
        #if PF == .RGB8  { pixDst[0] = r; pixDst[1] = g; pixDst[2] = b; }
        #if PF == .RGBA8 { pixDst[0] = r; pixDst[1] = g; pixDst[2] = b; pixDst[3] = a; }
    }
}