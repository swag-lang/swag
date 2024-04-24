#global public
using Swag, Core, Core.Errors, Core.StrConv

#[ConstExpr]
struct Color
{
    #if Core.BIG_ENDIAN
    {
        a, r, g, b: u8
        #[Offset("a")]
        argb: u32
    }
    #else
    {
        b, g, r, a: u8
        #[Offset("b")]
        argb: u32
    }
}

impl Color
{
    #[Complete, Inline, ConstExpr, Overload]
    func opAffect(self, argb: u32) = self.argb = argb
    #[Complete, Inline, Implicit, ConstExpr, Overload]
    func opAffect(self, argb: Argb) = self.argb = cast(u32) argb
    #[Complete, Inline, ConstExpr, Overload]
    func opAffect(self, name: string) = self.argb = cast(u32) Argb.fromName(name)

    #[Inline, Overload]
    func opEquals(self, argb: u32) => self.argb == argb
    #[Inline, Overload]
    func opEquals(self, argb: Argb) => self.argb == cast(u32) argb
    #[Inline, Overload]
    func opEquals(self, other: Self) => self.argb == other.argb

    #[Inline]
    mtd const isOpaque() => a == 255

    // Set all components
    #[Inline]
    func setArgb(self, a, r, g, b: u8)
    {
        self.a = a
        self.r = r
        self.g = g
        self.b = b
    }

    // Set all components with floats in the range [0 1]
    #[Inline]
    func setArgbf(self, a, r, g, b: f32)
    {
        self.a = acast (Math.clamp(a, 0, 1) * 255)
        self.r = acast (Math.clamp(r, 0, 1) * 255)
        self.g = acast (Math.clamp(g, 0, 1) * 255)
        self.b = acast (Math.clamp(b, 0, 1) * 255)
    }

    // Just set r, g and b components.
    // Alpha will be opaque.
    #[Inline, Overload]
    func setRgb(self, r, g, b: u8)
    {
        self.a = 255
        self.r = r
        self.g = g
        self.b = b
    }

    // Just set r, g and b components.
    // Alpha will be opaque.
    #[Inline, Overload]
    mtd setRgb(rgb: Color)
    {
        self.r = rgb.r
        self.g = rgb.g
        self.b = rgb.b
    }

    // Set r,g,b with floats in the range [0 1].
    // Alpha will be opaque.
    #[Inline]
    func setRgbf(self, r, g, b: f32)
    {
        self.a = 255
        self.r = acast (Math.clamp(r, 0, 1) * 255)
        self.g = acast (Math.clamp(g, 0, 1) * 255)
        self.b = acast (Math.clamp(b, 0, 1) * 255)
    }

    // Retrieve all components as floating point values between 0 and 1
    #[Inline]
    func toArgbf(const self)->{ a: f32, r: f32, g: f32, b: f32 }
    {
        var result: retval = undefined
        result.a = self.a / 255.0
        result.r = self.r / 255.0
        result.g = self.g / 255.0
        result.b = self.b / 255.0
        return result
    }

    // Get the color as a [Math.Vector4]
    #[Inline]
    func toVector4(const self)->Math.Vector4
    {
        var result: retval = undefined
        result.w = self.a / 255.0
        result.x = self.r / 255.0
        result.y = self.g / 255.0
        result.z = self.b / 255.0
        return result
    }

    // Retrieve all components as floating point values between 0 and 1
    #[Inline]
    func toRgbf(const self)->{ r: f32, g: f32, b: f32 }
    {
        var result: retval = undefined
        result.r = self.r / 255.0
        result.g = self.g / 255.0
        result.b = self.b / 255.0
        return result
    }

    // Create a new Color with specified values
    #[Inline]
    func fromRgb(r, g, b: u8)->Color
    {
        var result: retval = undefined
        result.setRgb(r, g, b)
        return result
    }

    // Create a new Color with specified values
    #[Inline]
    func fromRgbf(r, g, b: f32)->Color
    {
        var result: retval = undefined
        result.setRgbf(r, g, b)
        return result
    }

    // Create a new Color with specified values
    #[Inline, Overload]
    func fromArgb(a, r, g, b: u8)->Color
    {
        var result: retval = undefined
        result.setArgb(a, r, g, b)
        return result
    }

    // Create a new Color with specified values
    #[Inline, Overload]
    func fromArgb(argb: Argb)->Color
    {
        var result: retval = argb
        return result
    }

    // Create a new Color with specified values
    #[Inline, Overload]
    func fromArgb(alpha: u8, argb: Argb)->Color
    {
        var result: retval = argb
        result.a = alpha
        return result
    }

    // Create a new Color with specified values
    #[Inline, Overload]
    func fromArgb(argb: u32)->Color
    {
        var result: retval = argb
        return result
    }

    // Create a new Color with specified values
    #[Inline]
    func fromAbgr(abgr: u32)->Color
    {
        var result: retval
        result.a = acast (abgr >> 24)
        result.b = acast ((abgr >> 16) & 0xFF)
        result.g = acast ((abgr >> 8) & 0xFF)
        result.r = acast (abgr & 0xFF)
        return result
    }

    // Create a new Color with specified values
    #[Inline]
    func fromArgbf(a, r, g, b: f32)->Color
    {
        var result: retval = undefined
        result.setArgbf(a, r, g, b)
        return result
    }

    // Create a new Color with specified values
    #[Inline]
    func fromVector4(vec: Math.Vector4)->Color
    {
        var result: retval = undefined
        result.setArgbf(vec.w, vec.x, vec.y, vec.z)
        return result
    }

    // Create a new Color with specified values
    #[Inline]
    func fromHsl(h, s, l: f32)->Color
    {
        var result: retval = undefined
        result.setHsl(h, s, l)
        return result
    }

    // Initialize color with hue, saturation, lightness
    mtd setHsl(h, s, l: f32)
    {
        func hueToCol(m1, m2, hue: f32)->u8
        {
            var h = hue
            if h < 0 do
                h += 1
            if h > 1 do
                h -= 1

            var v: f32 = undefined
            if 6 * h < 1 do
                v = m1 + (m2 - m1) * h * 6
            elif 2 * h < 1 do
                v = m2
            elif 3 * h < 2 do
                v = m1 + (m2 - m1) * ((2.0 / 3.0) - h) * 6
            else do
                v = m1

            return cast(u8) (v * 255)
        }

        var _h = Math.clamp(h, 0.0, 1.0)
        var _l = Math.clamp(l, 0.0, 1.0)
        var _s = Math.clamp(s, 0.0, 1.0)

        if _s == 0
        {
            r = cast(u8) (255 * _l)
            g = r
            b = r
        }
        else
        {
            let m2 = _l < 0.5 ? _l * (1 + _s) : (_l + _s) - (_l * _s)
            let m1 = 2 * _l - m2
            r = hueToCol(m1, m2, _h + (1.0 / 3.0))
            g = hueToCol(m1, m2, _h)
            b = hueToCol(m1, m2, _h - (1.0 / 3.0))
        }
    }

    // Returns hue, saturation, lightness
    mtd const toHsl()->{ h: f32, s: f32, l: f32 }
    {
        var result: retval = undefined

        let (rf, gf, bf) = toRgbf()
        let max = Math.max(rf, gf, bf)
        let min = Math.min(rf, gf, bf)
        result.l = (max + min) * 0.5
        let delta = max - min
        if delta == 0
        {
            result.s = 0
            result.h = 0
            return result
        }

        if result.l < 0.5 do
            result.s = delta / (max + min)
        else do
            result.s = delta / (2 - max - min)

        let delr = (((max - rf) / 6) + (delta * 0.5)) / delta
        let delg = (((max - gf) / 6) + (delta * 0.5)) / delta
        let delb = (((max - bf) / 6) + (delta * 0.5)) / delta

        if rf == max do
            result.h = delb - delg
        elif gf == max do
            result.h = (1.0 / 3.0) + delr - delb
        else do
            result.h = (2.0 / 3.0) + delg - delr

        if result.h < 0 do
            result.h += 1
        if result.h > 1 do
            result.h -= 1

        return result
    }

    // Blend two colors, keeping the alpha channel of 'col0' untouched
    #[ConstExpr]
    func getBlendKeepAlpha(col0, col1: Color, factor: f32)->Color
    {
        var result: retval = undefined
        result.a = col0.a
        result.r = cast(u8) Math.lerp(cast(f32) col0.r, cast(f32) col1.r, factor)
        result.g = cast(u8) Math.lerp(cast(f32) col0.g, cast(f32) col1.g, factor)
        result.b = cast(u8) Math.lerp(cast(f32) col0.b, cast(f32) col1.b, factor)
        return result
    }

    // Blend two colors
    #[ConstExpr]
    func getBlend(col0, col1: Color, factor: f32)->Color
    {
        var result: retval = undefined
        result.a = cast(u8) Math.lerp(cast(f32) col0.a, cast(f32) col1.a, factor)
        result.r = cast(u8) Math.lerp(cast(f32) col0.r, cast(f32) col1.r, factor)
        result.g = cast(u8) Math.lerp(cast(f32) col0.g, cast(f32) col1.g, factor)
        result.b = cast(u8) Math.lerp(cast(f32) col0.b, cast(f32) col1.b, factor)
        return result
    }

    // Returns a darker color
    #[ConstExpr]
    func getDarker(col: Color, factor: f32)->Color
    {
        return col.getBlendKeepAlpha(Color.fromArgb(Argb.Black), factor)
    }

    // Returns a lighter color
    #[ConstExpr]
    func getLighter(col: Color, factor: f32)->Color
    {
        return col.getBlendKeepAlpha(Color.fromArgb(Argb.White), factor)
    }

    // Returns the distance between the other color
    mtd getDistanceRgb(col: Color)->f32
    {
        let x1   = cast(f32) r - col.r
        let x2   = cast(f32) g - col.g
        let x3   = cast(f32) b - col.b
        var dist = Math.sqrt((x1 * x1) + (x2 * x2) + (x3 * x3))
        dist /= 255 * 3
        return dist
    }
}

impl Core.StrConv.IPokeValue for Color
{
    mtd impl poke(buf: string)->string throw
    {
        var eat: u64
        var ovf: bool

        var one = Tokenize.eatSpaces(buf)
        if @countof(one) == 0 do
            throw SyntaxError{}

        if buf[0] == `0` and buf[1] | 32 == `x`
        {
            (argb, eat, ovf) = try parseU32(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)
        }
        elif Latin1.isDigit(buf[0])
        {
            (a, eat, ovf) = try parseU8(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (r, eat, ovf) = try parseU8(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (g, eat, ovf) = try parseU8(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (b, eat, ovf) = try parseU8(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)
        }
        else
        {
            let id = try parseIdentifier(one)
            one  = Tokenize.eatCount(one, @countof(id))
            argb = cast(u32) Argb.fromName(id)
        }

        return one
    }
}
