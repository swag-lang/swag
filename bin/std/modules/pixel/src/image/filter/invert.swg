using Core

impl Image
{
    // Invert colors
    public mtd invert()
    {
        // unsupported pixel format
        if .pf.channels() < 3 or .pf.bitDepth() != 8 do
            return

        var convArr: [256] u8 = undefined
        for i in 256 do
            convArr[i] = cast(u8) (255 - i)

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
