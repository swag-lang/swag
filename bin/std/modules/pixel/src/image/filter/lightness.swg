#global marked
using Core

impl Image
{
    // Change the lightness. 'factor' is [-1, 1]
    public mtd lightness(factor: f32 = 0.5)
    {
        // unsupported pixel format
        if me.pf.channels() < 3 or me.pf.bitDepth() != 8:
            return

        let level    = cast(s32) (Math.clamp(factor, -1, 1) * 255)
        var convArr: [256] u8 = undefined
        for i in 256:
            convArr[i] = cast(u8) Math.clamp(i + level, 0, 255)

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
