#global marked
using Core

impl Image
{
    // Invert colors
    public mtd invert()
    {
        // unsupported pixel format
        if me.pf.channels() < 3 or me.pf.bitDepth() != 8:
            return

        var convArr: [256] u8 = undefined
        for i in 256:
            convArr[i] = cast(u8) (255 - i)

        let f = &convArr[0]
        me.visitPixels(f)
        {
            let arr = cast(^u8) userData
            pix[0] = arr[pix[0]]
            pix[1] = arr[pix[1]]
            pix[2] = arr[pix[2]]
        }
    }
}
