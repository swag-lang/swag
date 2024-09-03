#global public
using Swag, Core, Core.Serialization

// An image buffer, in various pixel formats.
struct Image
{
    pixels:     Array'u8        // All the pixels
    size:       u64             // Size in bytes
    width:      s32             // The width of the image
    height:     s32             // The height of the image
    width8:     s32             // The width, in bytes
    pf:         PixelFormat     // Format of one pixel
    bpp:        u8              // Bits per pixel
    bpp8:       u8              // Bytes per pixel

    #[Core.Serialization.NoSerialize]
    workingBuffer: *Image // Temporary buffer for filters and transformations
}

impl Image
{
    // Returns true if the image is valid
    #[Swag.Inline]
    mtd const isValid() => pixels.buffer != null

    // Creates a new image
    func create(width, height: s32, pf: PixelFormat, buffer: const [..] u8 = null)->Image
    {
        Debug.assert(width > 0 and height > 0, "invalid dimensions")

        var result: retval
        result.init(width, height, pf)
        result.allocPixels()
        if buffer:
            Memory.copy(result.pixels.buffer, @dataof(buffer), @countof(buffer))
        return result
    }

    mtd opDrop()
    {
        Memory.delete'Image(workingBuffer)
    }

    mtd opPostCopy()
    {
        workingBuffer = null
    }

    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using self, stmt: code)
    {
        var scan = pixels.buffer
        for width * height
        {
            #macro
            {
                var #alias0 = #up scan
                var #alias1 = #index
                #inject #up stmt
            }

            scan += bpp8
        }
    }

    // Initialize image informations
    // 'pixels' is set to null, and must be initialized after a call to that function
    // This gives the opportunity to set the pixels with an external buffer
    mtd init(width, height: s32, pf: PixelFormat)
    {
        self.pixels.clear()
        self.width  = #self.width
        self.height = #self.height
        self.pf     = #self.pf

        bpp    = self.pf.bpp()
        bpp8   = cast(u8) (bpp >> 3)
        width8 = self.width * bpp8
        size   = cast(u64) (self.width * self.height * bpp8)
    }

    // Allocate pixels
    mtd allocPixels()
    {
        Debug.assert(size != 0)
        pixels.resize(size)
    }

    // Allocate a computing buffer with current image size
    #[Swag.Overload]
    mtd allocWorkingBuffer()
    {
        allocWorkingBuffer(width, height, pf)
    }

    // Allocate a computing buffer with new sizes
    #[Swag.Overload]
    mtd allocWorkingBuffer(newWidth, newHeight: s32, newPf: PixelFormat)
    {
        if workingBuffer and
           newWidth == workingBuffer.width and
           newHeight == workingBuffer.height and
           newPf == workingBuffer.pf
        {
            return
        }

        freeWorkingBuffer()
        workingBuffer      = Memory.new'Image()
        dref workingBuffer = Image.create(newWidth, newHeight, newPf)
    }

    // Free the working buffer if allocated
    mtd freeWorkingBuffer()
    {
        Memory.delete(workingBuffer)
        workingBuffer = null
    }

    // Replace the current image content with the working buffer
    mtd applyWorkingBuffer()
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

    // Macro to foreach all pixels of the image in parallel chunks.
    // Exported variables:
    // | pix       | address of the pixel
    // | image     | the processed image
    // | index     | the pixel index
    // | x         | the pixel x coordinates
    // | y         | the pixel y coordinates
    // | userData  | as passed to the macro
    #[Swag.Macro]
    mtd const visitPixels(userData: *void = null, stride: s32 = 1, stmt: code)
    {
        Debug.assert(stride > 0)

        var p = {self, userData}
        Jobs.parallelVisit(|buffer, data| pixels.toSlice(), cast(u32) (bpp8 * stride), &p)
        {
            let p = cast(*#decltype(p)) data
            #macro
            {
                let image    = #up p.self
                let userData = #up p.userData
                let pix      = #up buffer

                let index = cast(u64) (pix - image.pixels.buffer) / image.bpp8
                let x     = cast(s32) index % image.width
                let y     = cast(s32) index / image.width

                #inject #up stmt
            }
        }
    }

    // Release the content of the image
    mtd release()
    {
        @drop(self, 1)
        @init(self, 1)
    }

    // Clear the content of the image
    mtd clear()
    {
        Debug.assert(pixels.buffer != null)
        Memory.clear(pixels.buffer, size)
    }

    // Copy one pixel, depending on bpp
    #[Swag.Inline]
    func(BPP: u8) copyPixel(pixDst, pixSrc: ^u8)
    {
        #if BPP == 24
        {
            dref cast(*u16) pixDst = dref cast(*u16) pixSrc
            pixDst[2]              = pixSrc[2]
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
    func(PF: PixelFormat) setPixelRGBA8(pixDst: ^u8, r, g, b, _a: u8)
    {
        #if PF == .BGR8
        {
            pixDst[0] = b
            pixDst[1] = g
            pixDst[2] = r
        }
        #if PF == .BGRA8
        {
            pixDst[0] = b
            pixDst[1] = g
            pixDst[2] = r
            pixDst[3] = _a
        }
        #if PF == .RGB8
        {
            pixDst[0] = r
            pixDst[1] = g
            pixDst[2] = b
        }
        #if PF == .RGBA8
        {
            pixDst[0] = r
            pixDst[1] = g
            pixDst[2] = b
            pixDst[3] = _a
        }
    }

    // Returns the color at the given coordinate
    mtd getPixelColor(pixX, pixY: s32)->Color
    {
        var result: retval
        let x       = Math.clamp(pixX, 0, width - 1)
        var y       = Math.clamp(pixY, 0, height - 1)
        y = height - y - 1

        #[Swag.Complete]
        switch pf
        {
        case RGBA8:
            let pix = pixels.buffer + (x * 4) + (y * width * 4)
            result.r = pix[0]
            result.g = pix[1]
            result.b = pix[2]
            result.a = pix[3]
        case RGB8:
            let pix = pixels.buffer + (x * 3) + (y * width * 3)
            result.r = pix[0]
            result.g = pix[1]
            result.b = pix[2]
            result.a = 255
        case BGRA8:
            let pix = pixels.buffer + (x * 4) + (y * width * 4)
            result.b = pix[0]
            result.g = pix[1]
            result.r = pix[2]
            result.a = pix[3]
        case BGR8:
            let pix = pixels.buffer + (x * 3) + (y * width * 3)
            result.b = pix[0]
            result.g = pix[1]
            result.r = pix[2]
            result.a = 255
        }

        return result
    }
}
