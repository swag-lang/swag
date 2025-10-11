using Core

impl Image
{
    // Change the gamma.
    public mtd gamma(level: f32 = 2.2)
    {
        // unsupported pixel format
        if .pf.channels() < 3 or .pf.bitDepth() != 8 do
            return

        let g = 1.0 / Math.clamp(level, Math.ConstF32.Epsilon, 5)

        var convArr: [256] u8 = undefined
        for i in 256 do
            convArr[i] = cast(u8) (Math.pow(i / 255.0, g) * 255)

        let f = &convArr[0]
        .visitPixels(f)
        {
            let arr = cast([*] u8) userData
            pix[0] = arr[pix[0]]
            pix[1] = arr[pix[1]]
            pix[2] = arr[pix[2]]
        }
    }
}
