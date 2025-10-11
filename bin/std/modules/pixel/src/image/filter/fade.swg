using Core

impl Image
{
    // Fade to a given color. 'factor' is [0, 1]
    public mtd fade(color: Color, factor: f32 = 0.5)
    {
        // unsupported pixel format
        if .pf.channels() < 3 or .pf.bitDepth() != 8 do
            return

        var convArrR: [256] u8 = undefined
        var convArrG: [256] u8 = undefined
        var convArrB: [256] u8 = undefined

        if .pf == .BGR8 or .pf == .BGRA8
        {
            for i in 256
            {
                convArrR[i] = cast(u8) Math.lerp(cast(f32) i, cast(f32) color.b, factor)
                convArrG[i] = cast(u8) Math.lerp(cast(f32) i, cast(f32) color.g, factor)
                convArrB[i] = cast(u8) Math.lerp(cast(f32) i, cast(f32) color.r, factor)
            }
        }
        elif .pf == .RGB8 or .pf == .RGBA8
        {
            for i in 256
            {
                convArrR[i] = cast(u8) Math.lerp(cast(f32) i, cast(f32) color.r, factor)
                convArrG[i] = cast(u8) Math.lerp(cast(f32) i, cast(f32) color.g, factor)
                convArrB[i] = cast(u8) Math.lerp(cast(f32) i, cast(f32) color.b, factor)
            }
        }

        var f = {r: &convArrR[0], g: &convArrG[0], b: &convArrB[0]}

        .visitPixels(&f)
        {
            let arr = cast(*#decltype(f)) userData
            pix[0] = arr.r[pix[0]]
            pix[1] = arr.g[pix[1]]
            pix[2] = arr.b[pix[2]]
        }
    }
}
