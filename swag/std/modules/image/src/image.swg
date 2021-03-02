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
				#[swag.noreturn]
				#mixin `stmt
			}

            scan += bpp8
        }
    }

    #[swag.macro]
    func visitPixels(using self, userData: *void, stmt: code)
    {
        struct PixelJob
        {
            using base: jobsystem.Job
            pix:        [..] u8
            bpp8:       u8
            userData:   *void
        }

        func jobEntry(jobData: *void)
        {
            oneJob := cast(*PixelJob) jobData
            visitPixels(oneJob.pix, oneJob.bpp8, oneJob.userData)
            thread.sleep(100)
        }

        func visitPixels(pix: [..] u8, bpp8: u8, userData: *void)
        {
            scan := @dataof(pix)
            countof := @countof(pix)
            loop countof / bpp8
            {
                #macro
                {
                    pix  := `scan
                    data := `userData
                    #[swag.noreturn]
                    #mixin `stmt
                }

                scan += bpp8
            }
        }

        if jobsystem.isSynchrone()
        {
            visitPixels(@mkslice(pixels, size), bpp8, userData)
            return
        }

        //numWorkers := jobsystem.getNumWorkers()
        numWorkers := 50'u32
        wh := width * height
        splitSize := wh / numWorkers
        lastSplitSize := splitSize + (wh - (splitSize * numWorkers))
        splitSize *= bpp8
        lastSplitSize *= bpp8

        var jobs: Array'PixelJob
        jobs.resize(cast(uint) numWorkers)

        ptr := pixels
        visit *j: jobs
        {
            sizeSlice := @index == jobs.count - 1 ? lastSplitSize : splitSize

            j.base.lambda = &jobEntry
            j.base.data = j
            j.bpp8 = bpp8
            j.userData = userData
            j.pix = @mkslice(ptr, sizeSlice)
            ptr += sizeSlice

            jobsystem.scheduleJob(j)
        }

        visit *j: jobs jobsystem.waitJob(j)
        jobsystem.waitDone()
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

    // Returns the pixel at the given x,y coordinate
    func getPixel(using self, x, y: u32)->u64
    {
        addr := getPixelAddress(x, y)
        switch bpp
        {
        case 24:
            return cast(u64) (addr[0] | addr[1] << 8 | addr[2] << 16)
        case 32:
            return cast(u64) (:cast(*u32) addr)
        default:
            debug.assert(false)
        }
        return 0
    }

    // Set the pixel at the given x,y coordinate
    func setPixel(using self, x, y: u32, pixel: u64)
    {
        addr := getPixelAddress(x, y)
        switch bpp
        {
        case 24:
            addr[0] = bitcast(u8) pixel
            addr[1] = bitcast(u8) (pixel >>% 8)
            addr[2] = bitcast(u8) (pixel >>% 16)
        case 32:
            :cast(*u32) addr = cast(u32) pixel
        default:
            debug.assert(false)
        }
    }
}