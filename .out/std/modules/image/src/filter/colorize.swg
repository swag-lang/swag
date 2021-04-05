using core, drawing

impl Image
{
    // Colorize the image by setting the hue and saturation.
    #[swag.safety("", false)]
    public func colorize(using self, hue, saturation: f32, strength = 1.0)
    {
        debug.assert(pf == .BGR8 || pf == .BGRA8)

        h  := math.clamp(hue, 0.0, 1.0)
        s  := math.clamp(saturation, 0.0, 1.0)
        st := math.clamp(strength, 0.0, 1.0)

        // Precompute bgr depending on luminance
        var convArr: [256, 3] f32 = ?
        loop i: 256
        {
            c := Color.fromHsl(h, s, i / 255.0)
            convArr[i, 0] = cast(f32) c.b
            convArr[i, 1] = cast(f32) c.g
            convArr[i, 2] = cast(f32) c.r
        }

        f := @{&convArr[0, 0], st}
        visitPixels(&f)
        {
            params := cast(*@typeof(f)) userData

            p0 := pix[0]
            p1 := pix[1]
            p2 := pix[2]

            min := math.min(p0, p1, p2)
            max := math.max(p0, p1, p2)
            lum := cast(u8) ((max + min) * 0.5)

            precalc := params.item0 + (lum * 3)
            pix[0] = cast(u8) math.lerp(cast(f32) p0, precalc[0], params.st)
            pix[1] = cast(u8) math.lerp(cast(f32) p1, precalc[1], params.st)
            pix[2] = cast(u8) math.lerp(cast(f32) p2, precalc[2], params.st)
        }
    }
}