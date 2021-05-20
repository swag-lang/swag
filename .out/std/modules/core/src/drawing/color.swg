#global namespace Drawing
#global public
using Swag

struct Color
{
    #if BIG_ENDIAN
    {
        a, r, g, b: u8
        #[offset("a")]
        argb: u32
    }
    #else
    {
        b, g, r, a: u8
        #[offset("b")]
        argb: u32
    }
}

impl Color
{
    #[complete, inline]
    func opAffect(self, argb: u32)    = self.argb = argb
    #[complete, inline, implicit]
    func opAffect(self, argb: Argb)   = self.argb = cast(u32) argb
    #[complete, inline]
    func opAffect(self, name: string) = self.argb = cast(u32) Argb.fromName(name)

    #[inline]
    func opEquals(self, argb: u32)   => self.argb == argb
    #[inline]
    func opEquals(self, argb: Argb)  => self.argb == cast(u32) argb
    #[inline]
    func opEquals(self, other: Self) => self.argb == other.argb

    // Set all components
    #[inline]
    func setArgb(self, a, r, g, b: u8)
    {
        self.a = a
        self.r = r
        self.g = g
        self.b = b
    }

    // Set all components with floats in the range [0 1]
    #[inline]
    func setArgbf(self, a, r, g, b: f32)
    {
        self.a = acast (Math.clamp(a, 0, 1) * 255)
        self.r = acast (Math.clamp(r, 0, 1) * 255)
        self.g = acast (Math.clamp(g, 0, 1) * 255)
        self.b = acast (Math.clamp(b, 0, 1) * 255)
    }

    // Just set r, g and b components.
    // Alpha will be opaque
    #[inline]
    func setRgb(self, r, g, b: u8)
    {
        self.a = 255
        self.r = r
        self.g = g
        self.b = b
    }

    // Set r,g,b with floats in the range [0 1]
    // Alpha will be opaque
    #[inline]
    func setRgbf(self, r, g, b: f32)
    {
        self.a = 255
        self.r = acast (Math.clamp(r, 0, 1) * 255)
        self.g = acast (Math.clamp(g, 0, 1) * 255)
        self.b = acast (Math.clamp(b, 0, 1) * 255)
    }

    // Retreive all components as floating point values between 0 and 1
    #[inline]
    func toArgbf(const self) -> {a: f32, r: f32, g: f32, b: f32}
    {
        var result: retval = ?
        result.a = self.a / 255.0'f32
        result.r = self.r / 255.0'f32
        result.g = self.g / 255.0'f32
        result.b = self.b / 255.0'f32
        return result
    }

    // Retreive all components as floating point values between 0 and 1
    #[inline]
    func toRgbf(const self) -> {r: f32, g: f32, b: f32}
    {
        var result: retval = ?
        result.r = self.r / 255.0'f32
        result.g = self.g / 255.0'f32
        result.b = self.b / 255.0'f32
        return result
    }

    // Create a new Color with specified values
    #[inline]
    func fromRgb(r, g, b: u8)->Color
    {
        var result: retval = ?
        result.setRgb(r, g, b)
        return result
    }

    // Create a new Color with specified values
    #[inline]
    func fromRgbf(r, g, b: f32)->Color
    {
        var result: retval = ?
        result.setRgbf(r, g, b)
        return result
    }

    // Create a new Color with specified values
    #[inline]
    func fromArgb(a, r, g, b: u8)->Color
    {
        var result: retval = ?
        result.setArgb(a, r, g, b)
        return result
    }

    // Create a new Color with specified values
    #[inline]
    func fromArgb(argb: Argb)->Color
    {
        var result: retval = argb
        return result
    }

    // Create a new Color with specified values
    #[inline]
    func fromArgb(argb: u32)->Color
    {
        var result: retval = argb
        return result
    }

    // Create a new Color with specified values
    #[inline]
    func fromArgbf(a, r, g, b: f32)->Color
    {
        var result: retval = ?
        result.setArgbf(a, r, g, b)
        return result
    }

    // Create a new Color with specified values
    #[inline]
    func fromHsl(h, s, l: f32)->Color
    {
        var result: retval = ?
        result.setHsl(h, s, l)
        return result
    }

    // Initialize color with hue, saturation, lightness
    func setHsl(using self, h, s, l: f32)
    {
        func hueToCol(m1, m2, hue: f32)->u8
        {
            h := hue
            if h < 0 h += 1
            if h > 1 h -= 1

            var v: f32 = ?
            if 6 * h < 1
                v = m1 + (m2 - m1) * h * 6
            else if 2 * h < 1
                v = m2
            else if 3 * h < 2
                v = m1 + (m2 - m1) * ((2.0 / 3.0) - h) * 6
            else
                v = m1

            return cast(u8) (v * 255)
        }

        _h := Math.clamp(h, 0.0, 1.0)
        _l := Math.clamp(l, 0.0, 1.0)
        _s := Math.clamp(s, 0.0, 1.0)

        if _s == 0
        {
            r = cast(u8) (255 * _l)
            g = r
            b = r
        }
        else
        {
            m2 := _l < 0.5 ? _l * (1 + _s) : (_l + _s) - (_l * _s)
            m1 := 2 * _l - m2
            r = hueToCol(m1, m2, _h + (1.0 / 3.0))
            g = hueToCol(m1, m2, _h)
            b = hueToCol(m1, m2, _h - (1.0 / 3.0))
        }
    }

    // Returns hue, saturation, lightness
    func toHsl(using const self) -> {h: f32, s: f32, l: f32}
    {
        var result: retval = ?

        (rf, gf, bf) := toRgbf()
        max := Math.max(rf, gf, bf)
        min := Math.min(rf, gf, bf)
        result.l = (max + min) * 0.5
        delta := max - min
        if delta == 0
        {
            result.s = 0
            result.h = 0
            return result
        }

        if result.l < 0.5
            result.s = delta / (max + min)
        else
            result.s = delta / (2 - max - min)

        delr := (((max - rf) / 6) + (delta * 0.5)) / delta
        delg := (((max - gf) / 6) + (delta * 0.5)) / delta
        delb := (((max - bf) / 6) + (delta * 0.5)) / delta

        if rf == max
            result.h = delb - delg
        else if gf == max
            result.h = (1.0 / 3.0) + delr - delb
        else
            result.h = (2.0 / 3.0) + delg - delr

        if (result.h < 0)
            result.h += 1
        if (result.h > 1)
            result.h -= 1

        return result
    }

    // Blend two colors, keeping the alpha channel of 'col0' untouched
    func getBlendKeepAlpha(col0, col1: Color, factor: f32)->Color
    {
        var result: retval = ?
        result.a = col0.a
        result.r = cast(u8) Math.lerp(cast(f32) col0.r, cast(f32) col1.r, factor)
        result.g = cast(u8) Math.lerp(cast(f32) col0.g, cast(f32) col1.g, factor)
        result.b = cast(u8) Math.lerp(cast(f32) col0.b, cast(f32) col1.b, factor)
        return result
    }

    // Blend two colors
    func getBlend(col0, col1: Color, factor: f32)->Color
    {
        var result: retval = ?
        result.a = cast(u8) Math.lerp(cast(f32) col0.a, cast(f32) col1.a, factor)
        result.r = cast(u8) Math.lerp(cast(f32) col0.r, cast(f32) col1.r, factor)
        result.g = cast(u8) Math.lerp(cast(f32) col0.g, cast(f32) col1.g, factor)
        result.b = cast(u8) Math.lerp(cast(f32) col0.b, cast(f32) col1.b, factor)
        return result
    }
}
