using Core

impl Image
{
    public enum MixMode
    {
        Copy
        AlphaBlend
    }

    mtd const doMix(dstImage: *Image, srcImage: Image, x, y: s32, mode = MixMode.Copy)
    {
        Debug.assert(srcImage.pf.hasAlpha() or mode != .AlphaBlend)

        let mec = cast #unconst (*Image) me
        var f   = {dstImage, srcImage: &srcImage, x, y, mode}

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            mec.visitPixels(&f)
            {
                let params   = cast(*#decltype(f)) userData
                let dstImage = cast(*Image) params.dstImage
                let srcImage = cast(const *Image) params.srcImage

                var pixDst = dstImage.pixels.buffer + x * image.bpp8
                pixDst += cast(u64) (y * (image.width * image.bpp8))

                // fast path: outside blit rect, keep pixel as-is
                if x < params.x or y < params.y or
                   x >= params.x + srcImage.width or
                   y >= params.y + srcImage.height
                {
                    copyPixelDirect'BPP(pixDst, pix) // copy current dst pixel back (no change)
                }
                else
                {
                    let x1     = x - params.x
                    let y1     = y - params.y
                    var pixSrc = srcImage.pixels.buffer + x1 * srcImage.bpp8
                    pixSrc += cast(u64) (y1 * (srcImage.width * srcImage.bpp8))

                    #[Swag.Complete]
                    switch params.mode
                    {
                    case Copy:
                        copyPixelDirect'BPP(pixDst, pixSrc)

                    case AlphaBlend:
                        if BPP != 32 do
                            copyPixelDirect'BPP(pixDst, pixSrc)
                        else
                        {
                            let bf = pixSrc[3] / 255.0
                            pixDst[0] = cast(u8) Math.lerp(cast(f32) pix[0], cast(f32) pixSrc[0], bf)
                            pixDst[1] = cast(u8) Math.lerp(cast(f32) pix[1], cast(f32) pixSrc[1], bf)
                            pixDst[2] = cast(u8) Math.lerp(cast(f32) pix[2], cast(f32) pixSrc[2], bf)
                            pixDst[3] = pix[3]
                        }
                    }
                }
            }
        }

        switch .bpp
        {
        case 24:
            doIt'24()
        case 32:
            doIt'32()
        default:
            Debug.assert(false)
        }
    }

    // Mix with another image
    public mtd mix(srcImage: Image, x, y: s32, mode = MixMode.Copy)
    {
        Debug.assert(.pf == srcImage.pf, "mismatched pixel format")
        Debug.assert(.pixels.buffer != null)
        .allocWorkingBuffer()
        .doMix(.workingBuffer, srcImage, x, y, mode)
        .applyWorkingBuffer()
    }

    // Mix with another image
    public mtd const toMix(srcImage: Image, x, y: s32, mode = MixMode.Copy)->Image
    {
        Debug.assert(.pixels.buffer != null)
        Debug.assert(.pf == srcImage.pf, "mismatched pixel format")
        var result: retval = create(.width, .height, .pf)
        .doMix(&result, srcImage, x, y, mode)
        return result
    }
}
