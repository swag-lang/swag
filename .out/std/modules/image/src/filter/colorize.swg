using core, Drawing

impl Image
{
    // Colorize the image by setting the hue and saturation.
    #[Swag.safety("", false)]
    public func colorize(using self, hue, saturation: f32, strength = 1.0)
    {
        // unsupported pixel format
        if pf.channels() < 3 or pf.bitDepth() != 8
            return

        h  := math.clamp(hue, 0.0, 1.0)
        s  := math.clamp(saturation, 0.0, 1.0)
        st := math.clamp(strength, 0.0, 1.0)

        // Precompute bgr depending on luminance
        var convArr: [256, 3] f32 = ?

        if pf == .BGR8 or pf == .BGRA8
        {
            loop i: 256
            {
                c := Color.fromHsl(h, s, i / 255.0)
                convArr[i, 0] = cast(f32) c.b
                convArr[i, 1] = cast(f32) c.g
                convArr[i, 2] = cast(f32) c.r
            }
        }
        else if pf == .RGB8 or pf == .RGBA8
        {
            loop i: 256
            {
                c := Color.fromHsl(h, s, i / 255.0)
                convArr[i, 0] = cast(f32) c.r
                convArr[i, 1] = cast(f32) c.g
                convArr[i, 2] = cast(f32) c.b
            }
        }

        f := @{&convArr[0, 0], st}
        visitPixels(&f)
        {
            params := cast(*@typeof(f)) userData

            min := math.min(pix[0], pix[1], pix[2])
            max := math.max(pix[0], pix[1], pix[2])
            lum := cast(u8) ((max + min) / 2)

            precalc := params.item0 + (lum * 3)
            pix[0] = cast(u8) math.lerp(cast(f32) pix[0], precalc[0], params.st)
            pix[1] = cast(u8) math.lerp(cast(f32) pix[1], precalc[1], params.st)
            pix[2] = cast(u8) math.lerp(cast(f32) pix[2], precalc[2], params.st)
        }
    }
}