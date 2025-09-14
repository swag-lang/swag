#global public
using Swag, Core, Core.Errors, Core.StrConv

// Represents a color with 8-bit ARGB components.
// Provides utilities for color creation, conversion, and manipulation.
#[ConstExpr, Core.Serialization.Final]
struct Color
{
    #if Core.BIG_ENDIAN
    {
        a, r, g, b: u8 // Alpha, red, green, blue components (0-255)
        #[Offset("a")]
        argb: u32 // 32-bit ARGB representation
    }
    #else
    {
        b, g, r, a: u8 // Blue, green, red, alpha components (0-255)
        #[Offset("b")]
        argb: u32 // 32-bit ARGB representation
    }
}

impl Color
{
    // Assignment operator for u32 ARGB value.
    #[Complete, Inline, ConstExpr, Overload]
    func opAffect(me, argb: u32) => me.argb = argb

    // Assignment operator for 'Argb' enum value.
    #[Complete, Inline, Implicit, ConstExpr, Overload]
    func opAffect(me, argb: Argb) => me.argb = cast(u32) argb

    // Assignment operator for color name string.
    #[Complete, Inline, ConstExpr, Overload]
    func opAffect(me, name: string) => me.argb = cast(u32) Argb.fromName(name)

    // Equality comparison with u32 ARGB value.
    #[Inline, Overload]
    func opEquals(me, argb: u32) => me.argb == argb

    // Equality comparison with 'Argb' enum value.
    #[Inline, Overload]
    func opEquals(me, argb: Argb) => me.argb == cast(u32) argb

    // Equality comparison with another Color.
    #[Inline, Overload]
    func opEquals(me, other: Color) => me.argb == other.argb

    // Check if the color is fully opaque.
    #[Inline]
    mtd const isOpaque() => me.a == 255

    // Set all ARGB components with u8 values (0-255).
    #[Inline]
    func setArgb(me, a, r, g, b: u8)
    {
        me.a = a
        me.r = r
        me.g = g
        me.b = b
    }

    // Set all ARGB components with floats in the range [0, 1].
    #[Inline]
    func setArgbf(me, a, r, g, b: f32)
    {
        me.a = cast() (Math.clamp(a, 0, 1) * 255)
        me.r = cast() (Math.clamp(r, 0, 1) * 255)
        me.g = cast() (Math.clamp(g, 0, 1) * 255)
        me.b = cast() (Math.clamp(b, 0, 1) * 255)
    }

    // Set RGB components with u8 values (0-255).
    // Alpha will be set to opaque (255).
    #[Inline, Overload]
    func setRgb(me, r, g, b: u8)
    {
        me.a = 255
        me.r = r
        me.g = g
        me.b = b
    }

    // Copy RGB components from another color.
    // Alpha remains unchanged.
    #[Inline, Overload]
    mtd setRgb(rgb: Color)
    {
        me.r = rgb.r
        me.g = rgb.g
        me.b = rgb.b
    }

    // Set RGB components with floats in the range [0, 1].
    // Alpha will be set to opaque (255).
    #[Inline]
    func setRgbf(me, r, g, b: f32)
    {
        me.a = 255
        me.r = cast() (Math.clamp(r, 0, 1) * 255)
        me.g = cast() (Math.clamp(g, 0, 1) * 255)
        me.b = cast() (Math.clamp(b, 0, 1) * 255)
    }

    // Retrieve all ARGB components as floating point values between 0 and 1.
    #[Inline, ConstExpr]
    func toArgbf(const me)->{ a: f32, r: f32, g: f32, b: f32 }
    {
        var result: retval = undefined
        result.a = me.a / 255.0
        result.r = me.r / 255.0
        result.g = me.g / 255.0
        result.b = me.b / 255.0
        return result
    }

    // Convert color to a Math.Vector4 (w=alpha, x=red, y=green, z=blue).
    #[Inline, ConstExpr]
    func toVector4(const me)->Math.Vector4
    {
        var result: retval = undefined
        result.w = me.a / 255.0
        result.x = me.r / 255.0
        result.y = me.g / 255.0
        result.z = me.b / 255.0
        return result
    }

    // Retrieve RGB components as floating point values between 0 and 1.
    #[Inline, ConstExpr]
    func toRgbf(const me)->{ r: f32, g: f32, b: f32 }
    {
        var result: retval = undefined
        result.r = me.r / 255.0
        result.g = me.g / 255.0
        result.b = me.b / 255.0
        return result
    }

    // Create a new Color with specified RGB values and opaque alpha.
    #[Inline, ConstExpr]
    func fromRgb(r, g, b: u8)->Color
    {
        var result: retval = undefined
        result.setRgb(r, g, b)
        return result
    }

    // Create a new Color with specified RGB float values and opaque alpha.
    #[Inline, ConstExpr]
    func fromRgbf(r, g, b: f32)->Color
    {
        var result: retval = undefined
        result.setRgbf(r, g, b)
        return result
    }

    // Create a new Color with specified ARGB values.
    #[Inline, Overload, ConstExpr]
    func fromArgb(a, r, g, b: u8)->Color
    {
        var result: retval = undefined
        result.setArgb(a, r, g, b)
        return result
    }

    // Create a new Color from 'Argb' enum value.
    #[Inline, Overload, ConstExpr]
    func fromArgb(argb: Argb)->Color
    {
        let result: retval = argb
        return result
    }

    // Create a new Color from 'Argb' enum value with custom alpha.
    #[Inline, Overload, ConstExpr]
    func fromArgb(alpha: u8, argb: Argb)->Color
    {
        var result: retval = argb
        result.a = alpha
        return result
    }

    // Create a new Color from u32 ARGB value.
    #[Inline, Overload, ConstExpr]
    func fromArgb(argb: u32)->Color
    {
        let result: retval = argb
        return result
    }

    // Create a new Color from u32 ABGR value (reversed byte order).
    #[Inline, ConstExpr]
    func fromAbgr(abgr: u32)->Color
    {
        var result: retval
        result.a = cast() (abgr >> 24)
        result.b = cast() ((abgr >> 16) & 0xFF)
        result.g = cast() ((abgr >> 8) & 0xFF)
        result.r = cast() (abgr & 0xFF)
        return result
    }

    // Create a new Color with specified ARGB float values.
    #[Inline, ConstExpr]
    func fromArgbf(a, r, g, b: f32)->Color
    {
        var result: retval = undefined
        result.setArgbf(a, r, g, b)
        return result
    }

    // Create a new Color from Math.Vector4 (w=alpha, x=red, y=green, z=blue).
    #[Inline, ConstExpr]
    func fromVector4(vec: Math.Vector4)->Color
    {
        var result: retval = undefined
        result.setArgbf(vec.w, vec.x, vec.y, vec.z)
        return result
    }

    // Create a new Color from HSL values (hue, saturation, lightness).
    #[Inline, ConstExpr]
    func fromHsl(h, s, l: f32)->Color
    {
        var result: retval = undefined
        result.setHsl(h, s, l)
        return result
    }

    // Set color from HSL values (hue, saturation, lightness in range [0, 1]).
    mtd setHsl(h, s, l: f32)
    {
        // Convert hue component to RGB color value.
        func hueToCol(m1, m2, hue: f32)->u8
        {
            var h = hue
            if h < 0:
                h += 1
            if h > 1:
                h -= 1

            var v: f32 = undefined
            if 6 * h < 1:
                v = m1 + (m2 - m1) * h * 6
            elif 2 * h < 1:
                v = m2
            elif 3 * h < 2:
                v = m1 + (m2 - m1) * ((2.0 / 3.0) - h) * 6
            else:
                v = m1

            return cast(u8) (v * 255)
        }

        var _h = Math.clamp(h, 0.0, 1.0)
        var _l = Math.clamp(l, 0.0, 1.0)
        var _s = Math.clamp(s, 0.0, 1.0)

        if _s == 0
        {
            me.r = cast(u8) (255 * _l)
            me.g = me.r
            me.b = me.r
        }
        else
        {
            let m2 = _l < 0.5 ? _l * (1 + _s) : (_l + _s) - (_l * _s)
            let m1 = 2 * _l - m2
            me.r = hueToCol(m1, m2, _h + (1.0 / 3.0))
            me.g = hueToCol(m1, m2, _h)
            me.b = hueToCol(m1, m2, _h - (1.0 / 3.0))
        }
    }

    // Convert color to HSL values (hue, saturation, lightness in range [0, 1]).
    mtd const toHsl()->{ h: f32, s: f32, l: f32 }
    {
        var result: retval = undefined

        let (rf, gf, bf) = me.toRgbf()
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

        if result.l < 0.5:
            result.s = delta / (max + min)
        else:
            result.s = delta / (2 - max - min)

        let delr = (((max - rf) / 6) + (delta * 0.5)) / delta
        let delg = (((max - gf) / 6) + (delta * 0.5)) / delta
        let delb = (((max - bf) / 6) + (delta * 0.5)) / delta

        if rf == max:
            result.h = delb - delg
        elif gf == max:
            result.h = (1.0 / 3.0) + delr - delb
        else:
            result.h = (2.0 / 3.0) + delg - delr

        if result.h < 0:
            result.h += 1
        if result.h > 1:
            result.h -= 1

        return result
    }

    // Blend two colors using linear interpolation, keeping 'col0's alpha channel.
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

    // Blend two colors using linear interpolation (including alpha).
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

    // Returns a darker version of the color by blending with black.
    #[ConstExpr]
    func getDarker(col: Color, factor: f32)->Color
    {
        return col.getBlendKeepAlpha(Color.fromArgb(Argb.Black), factor)
    }

    // Returns a lighter version of the color by blending with white.
    #[ConstExpr]
    func getLighter(col: Color, factor: f32)->Color
    {
        return col.getBlendKeepAlpha(Color.fromArgb(Argb.White), factor)
    }

    // Calculate the RGB distance between this color and another (normalized to [0, 1]).
    mtd const getDistanceRgb(col: Color)->f32
    {
        let x1   = cast(f32) me.r - col.r
        let x2   = cast(f32) me.g - col.g
        let x3   = cast(f32) me.b - col.b
        var dist = Math.sqrt((x1 * x1) + (x2 * x2) + (x3 * x3))
        dist /= Math.sqrt(255.0 * 255.0 * 3.0) // Normalize to [0, 1]
        return dist
    }

    // Check if the color is considered "dark" based on luminance.
    #[Inline]
    mtd const isDark()->bool
    {
        return me.getGrayScalef() < 0.5
    }

    // Check if the color is considered "light" based on luminance.
    #[Inline]
    mtd const isLight()->bool
    {
        return me.getGrayScalef() >= 0.5
    }

    // Get a contrasting color (black or white) for text readability.
    #[Inline, ConstExpr]
    mtd const getContrasting()->Color
    {
        return me.isDark() ? Color.fromArgb(Argb.White) : Color.fromArgb(Argb.Black)
    }

    // Calculate the perceived brightness using luminance formula.
    #[Inline]
    mtd const getGrayScalef()->f32
    {
        return (cast(f32) me.r * 0.299 + cast(f32) me.g * 0.587 + cast(f32) me.b * 0.114) / 255.0
    }

    // Convert to grayscale color preserving alpha.
    #[Inline, ConstExpr]
    mtd const toGrayscale()->Color
    {
        let gray = cast(u8) me.getGrayScalef() * 255
        return Color.fromArgb(me.a, gray, gray, gray)
    }

    // Adjust saturation (-1.0 = grayscale, 0.0 = no change, 1.0 = full saturation).
    #[ConstExpr]
    mtd const adjustSaturation(factor: f32)->Color
    {
        let hsl    = me.toHsl()
        let newS   = Math.clamp(hsl.s + factor, 0.0, 1.0)
        var result = Color.fromHsl(hsl.h, newS, hsl.l)
        result.a = me.a // Preserve alpha
        return result
    }

    // Adjust brightness (-1.0 = black, 0.0 = no change, 1.0 = white).
    #[ConstExpr]
    mtd const adjustBrightness(factor: f32)->Color
    {
        let hsl    = me.toHsl()
        let newL   = Math.clamp(hsl.l + factor, 0.0, 1.0)
        var result = Color.fromHsl(hsl.h, hsl.s, newL)
        result.a = me.a // Preserve alpha
        return result
    }

    // Adjust hue by rotating around the color wheel.
    #[ConstExpr]
    mtd const rotateHue(degrees: f32)->Color
    {
        let hsl  = me.toHsl()
        var newH = hsl.h + (degrees / 360.0)
        if newH > 1.0:
            newH -= 1.0
        elif newH < 0.0:
            newH += 1.0
        var result = Color.fromHsl(newH, hsl.s, hsl.l)
        result.a = me.a // Preserve alpha
        return result
    }

    // Create a color with modified alpha.
    #[Inline, ConstExpr]
    mtd const withAlpha(alpha: u8)->Color
    {
        return fromArgb(alpha, me.r, me.g, me.b)
    }

    // Create a color with modified alpha (float version).
    #[Inline, ConstExpr]
    mtd const withAlphaf(alpha: f32)->Color
    {
        return fromArgb(cast(u8) (Math.clamp(alpha, 0.0, 1.0) * 255), me.r, me.g, me.b)
    }

    // Get complementary color (opposite on color wheel).
    #[ConstExpr]
    mtd const getComplementary()->Color
    {
        return me.rotateHue(180.0)
    }

    // Multiply color by another color (component-wise).
    #[ConstExpr]
    func multiply(col0, col1: Color)->Color
    {
        var result: retval = undefined
        result.a = cast(u8) ((cast(u16) col0.a * col1.a) / 255)
        result.r = cast(u8) ((cast(u16) col0.r * col1.r) / 255)
        result.g = cast(u8) ((cast(u16) col0.g * col1.g) / 255)
        result.b = cast(u8) ((cast(u16) col0.b * col1.b) / 255)
        return result
    }

    // Add colors with clamping.
    #[ConstExpr]
    func add(col0, col1: Color)->Color
    {
        var result: retval = undefined
        result.a = cast(u8) Math.min(cast(u16) col0.a + col1.a, 255)
        result.r = cast(u8) Math.min(cast(u16) col0.r + col1.r, 255)
        result.g = cast(u8) Math.min(cast(u16) col0.g + col1.g, 255)
        result.b = cast(u8) Math.min(cast(u16) col0.b + col1.b, 255)
        return result
    }
}

// Implementation of string parsing interface for Color values.
impl Core.StrConv.IPokeValue for Color
{
    // Parse a Color from a string representation.
    // Supports hex format (0x...), individual ARGB components, or named colors.
    mtd impl poke(buf: string)->string throw
    {
        var eat: u64
        var ovf: bool

        var one = Tokenize.eatSpaces(buf)
        if @countof(one) == 0:
            throw SyntaxError{}

        if buf[0] == '0' and buf[1] | 32 == 'x'
        {
            (me.argb, eat, ovf) = try parseU32(one)
            if ovf:
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)
        }
        elif Latin1.isDigit(buf[0])
        {
            (me.a, eat, ovf) = try parseU8(one)
            if ovf:
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (me.r, eat, ovf) = try parseU8(one)
            if ovf:
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (me.g, eat, ovf) = try parseU8(one)
            if ovf:
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (me.b, eat, ovf) = try parseU8(one)
            if ovf:
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)
        }
        else
        {
            let id = try parseIdentifier(one)
            one     = Tokenize.eatCount(one, @countof(id))
            me.argb = cast(u32) Argb.fromName(id)
        }

        return one
    }
}
