#global public
using Swag, Core

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

impl Image
{
    // Creates a new image
    func create(width, height: s32, pf: PixelFormat)->Image
    {
        Debug.assert(width > 0 and height > 0, "invalid dimensions")

        var result: retval
        result.init(width, height, pf)
        result.allocPixels()
        return result
    }

    #[Inline]
    private func free(using self)
    {
        Memory.free(pixels, size, allocator)
        Memory.delete'Image(workingBuffer, allocator)
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
            newPixels := Memory.alloc(size, allocator)
            Memory.copy(newPixels, pixels, size)
            pixels = newPixels
        }
    }

    #[Swag.macro]
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

    // Initialize image informations
    // 'pixels' is set to null, and must be initialized after a call to that function
    // This gives the opportunity to set the pixels with an external buffer
    func init(using self, width, height: s32, pf: PixelFormat)
    {
        self.pixels = null
        self.width = #scopefct.width
        self.height = #scopefct.height
        self.pf = #scopefct.pf

        bpp    = self.pf.bpp()
        bpp8   = cast(u8) (bpp >> 3)
        width8 = self.width * bpp8
        size   = cast(uint) (self.width * self.height * bpp8)
    }

    // Allocate pixels
    func allocPixels(using self)
    {
        Debug.assert(pixels == null)
        Debug.assert(size != 0)
        allocator = @getcontext().allocator
        pixels = Memory.alloc(size, allocator)
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
        workingBuffer = Memory.new'Image(allocator)
        dref workingBuffer = Image.create(newWidth, newHeight, newPf)
    }

    // Free the working buffer if allocated
    func freeWorkingBuffer(using self)
    {
        Memory.delete(workingBuffer, allocator)
        workingBuffer = null
    }

    // Replace the current image content with the working buffer
    func applyWorkingBuffer(using self)
    {
        Debug.assert(workingBuffer != null)
        swap(&pf, &workingBuffer.pf)
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
    #[Swag.macro]
    func visitPixels(using self, userData: *void = null, stride: s32 = 1, stmt: code)
    {
        Debug.assert(stride > 0)

        p := @{self, userData}
        Jobs.parallelVisit(@mkslice(pixels, size), cast(u32) bpp8 * stride, &p)
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
        Memory.clear(pixels, size)
    }

    // Copy one pixel, depending on bpp
    #[Swag.Inline]
    func(BPP: u8) copyPixel(pixDst, pixSrc: *u8)
    {
        #if BPP == 24
        {
            dref cast(*u16) pixDst = dref cast(*u16) pixSrc
            pixDst[2] = pixSrc[2]
        }
        #elif BPP == 32
        {
            dref cast(*u32) pixDst = dref cast(*u32) pixSrc
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