using Core

impl Image
{
    // Colorize the image by setting the hue and saturation.
    #[Swag.Safety("", false)]
    public mtd colorize(hue, saturation: f32, strength = 1.0)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        var h = Math.clamp(hue, 0.0, 1.0)
        var s = Math.clamp(saturation, 0.0, 1.0)
        var st = Math.clamp(strength, 0.0, 1.0)

        // Precompute bgr depending on luminance
        var convArr: [256, 3] f32 = undefined

        if pf == .BGR8 or pf == .BGRA8
        {
            loop i: 256
            {
                var c = Color.fromHsl(h, s, i / 255.0)
                convArr[i, 0] = cast(f32) c.b
                convArr[i, 1] = cast(f32) c.g
                convArr[i, 2] = cast(f32) c.r
            }
        }
        elif pf == .RGB8 or pf == .RGBA8
        {
            loop i: 256
            {
                var c = Color.fromHsl(h, s, i / 255.0)
                convArr[i, 0] = cast(f32) c.r
                convArr[i, 1] = cast(f32) c.g
                convArr[i, 2] = cast(f32) c.b
            }
        }

        var f = {&convArr[0, 0], st}
        visitPixels(&f)
        {
            var params = cast(*@decltype(f)) userData

            var min = Math.min(pix[0], pix[1], pix[2])
            var max = Math.max(pix[0], pix[1], pix[2])
            var lum = cast(u8) ((max + min) / 2)

            var precalc = params.item0 + (lum * 3)
            pix[0] = cast(u8) Math.lerp(cast(f32) pix[0], precalc[0], params.st)
            pix[1] = cast(u8) Math.lerp(cast(f32) pix[1], precalc[1], params.st)
            pix[2] = cast(u8) Math.lerp(cast(f32) pix[2], precalc[2], params.st)
        }
    }
}