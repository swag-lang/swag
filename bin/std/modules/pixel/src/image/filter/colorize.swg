using Core

impl Image
{
    // Colorize the image by setting the hue and saturation.
    #[Swag.Safety(.All, .All, false)]
    public mtd colorize(hue, saturation: f32, strength = 1.0)
    {
        // unsupported pixel format
        if .pf.channels() < 3 or .pf.bitDepth() != 8 do
            return

        let h  = Math.clamp(hue, 0.0, 1.0)
        let s  = Math.clamp(saturation, 0.0, 1.0)
        let st = Math.clamp(strength, 0.0, 1.0)

        // Precompute bgr depending on luminance
        var convArr: [256, 3] f32 = undefined

        if .pf == .BGR8 or .pf == .BGRA8
        {
            for i in 256
            {
                let c = Color.fromHsl(h, s, i / 255.0)
                convArr[i, 0] = cast(f32) c.b
                convArr[i, 1] = cast(f32) c.g
                convArr[i, 2] = cast(f32) c.r
            }
        }
        elif .pf == .RGB8 or .pf == .RGBA8
        {
            for i in 256
            {
                let c = Color.fromHsl(h, s, i / 255.0)
                convArr[i, 0] = cast(f32) c.r
                convArr[i, 1] = cast(f32) c.g
                convArr[i, 2] = cast(f32) c.b
            }
        }

        var f = {&convArr[0, 0], st}
        .visitPixels(&f)
        {
            let params = cast(*#decltype(f)) userData

            let min = Math.min(pix[0], pix[1], pix[2])
            let max = Math.max(pix[0], pix[1], pix[2])
            let lum = cast(u8) ((max + min) / 2)

            let precalc = params.item0 + (lum * 3)
            pix[0] = cast(u8) Math.lerp(cast(f32) pix[0], precalc[0], params.st)
            pix[1] = cast(u8) Math.lerp(cast(f32) pix[1], precalc[1], params.st)
            pix[2] = cast(u8) Math.lerp(cast(f32) pix[2], precalc[2], params.st)
        }
    }
}
