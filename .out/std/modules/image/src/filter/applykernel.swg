using core

impl Image
{
    public enum Kernel: const [..] s32
    {
        GaussianBlur3x3A = @[1, 8, 0, 1, 0, 1, 4, 1, 0, 1, 0]
    }

    // Transform image to grayscale, with a given factor
    public func applyKernel(using self, kernel: const [..] s32)
    {
        debug.assert(pf == PixelFormat.BGR8 || pf == PixelFormat.BGRA8)

        f := @{kernel}
        visitPixels(&f)
        {
            params := cast(*@typeof(f)) userData
            kernelSize := params.kernel[0]
            w8 := image.width * image.bpp8
            ks := @dataof(params.kernel) + 2

            var bs, gs, rs: s32
            for dy := -kernelSize; dy <= kernelSize; dy += 1
            {
                ny := math.clamp(cast(s32) y + dy, 0, cast(s32) image.height - 1)
                for dx := -kernelSize; dx <= kernelSize; dx += 1
                {
                    nx := math.clamp(cast(s32) x + dx, 0, cast(s32) image.width - 1)
                    ptr := image.pixels + (nx * image.bpp8) + (ny * w8)
                    bs += cast(s32) ptr[0] * :ks
                    gs += cast(s32) ptr[1] * :ks
                    rs += cast(s32) ptr[2] * :ks
                    ks += 1
                }
            }

            bs /= params.kernel[1]
            gs /= params.kernel[1]
            rs /= params.kernel[1]
            pix[0] = cast(u8) math.clamp(bs, 0, 255)
            pix[1] = cast(u8) math.clamp(gs, 0, 255)
            pix[2] = cast(u8) math.clamp(rs, 0, 255)
        }
    }
}