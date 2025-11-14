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
    func opAffect(me, argb: u32) { .argb = argb }

    // Assignment operator for 'Argb' enum value.
    #[Complete, Inline, Implicit, ConstExpr, Overload]
    func opAffect(me, argb: Argb) { .argb = cast(u32) argb }

    // Assignment operator for color name string.
    #[Complete, Inline, ConstExpr, Overload]
    func opAffect(me, name: string) { .argb = cast(u32) Argb.fromName(name) }

    // Equality comparison with u32 ARGB value.
    #[Inline, Overload]
    func opEquals(me, argb: u32) => .argb == argb

    // Equality comparison with 'Argb' enum value.
    #[Inline, Overload]
    func opEquals(me, argb: Argb) => .argb == cast(u32) argb

    // Equality comparison with another Color.
    #[Inline, Overload]
    func opEquals(me, other: Color) => .argb == other.argb

    // Check if the color is fully opaque.
    #[Inline]
    mtd const isOpaque() => .a == 255

    // Set all ARGB components with u8 values (0-255).
    #[Inline]
    func setArgb(me, a, r, g, b: u8)
    {
        .a = a
        .r = r
        .g = g
        .b = b
    }

    // Set all ARGB components with floats in the range [0, 1].
    #[Inline]
    func setArgbf(me, a, r, g, b: f32)
    {
        .a = cast() (Math.saturate(a) * 255)
        .r = cast() (Math.saturate(r) * 255)
        .g = cast() (Math.saturate(g) * 255)
        .b = cast() (Math.saturate(b) * 255)
    }

    // Set RGB components with u8 values (0-255).
    // Alpha will be set to opaque (255).
    #[Inline, Overload]
    func setRgb(me, r, g, b: u8)
    {
        .a = 255
        .r = r
        .g = g
        .b = b
    }

    // Copy RGB components from another color.
    // Alpha remains unchanged.
    #[Inline, Overload]
    mtd setRgb(rgb: Color)
    {
        .r = rgb.r
        .g = rgb.g
        .b = rgb.b
    }

    // Set RGB components with floats in the range [0, 1].
    // Alpha will be set to opaque (255).
    #[Inline]
    func setRgbf(me, r, g, b: f32)
    {
        .a = 255
        .r = cast() (Math.saturate(r) * 255)
        .g = cast() (Math.saturate(g) * 255)
        .b = cast() (Math.saturate(b) * 255)
    }

    // Retrieve all ARGB components as floating point values between 0 and 1.
    #[Inline, ConstExpr]
    mtd const toArgbf()->{ a: f32, r: f32, g: f32, b: f32 }
    {
        var result: retval = undefined
        result.a = .a / 255.0
        result.r = .r / 255.0
        result.g = .g / 255.0
        result.b = .b / 255.0
        return result
    }

    // Convert color to a Math.Vector4 (w=alpha, x=red, y=green, z=blue).
    #[Inline, ConstExpr]
    mtd const toVector4()->Math.Vector4
    {
        var result: retval = undefined
        result.w = .a / 255.0
        result.x = .r / 255.0
        result.y = .g / 255.0
        result.z = .b / 255.0
        return result
    }

    // Retrieve RGB components as floating point values between 0 and 1.
    #[Inline, ConstExpr]
    mtd const toRgbf()->{ r: f32, g: f32, b: f32 }
    {
        var result: retval = undefined
        result.r = .r / 255.0
        result.g = .g / 255.0
        result.b = .b / 255.0
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

        var _h = Math.saturate(h)
        var _l = Math.saturate(l)
        var _s = Math.saturate(s)

        if _s == 0
        {
            .r = cast(u8) (255 * _l)
            .g = .r
            .b = .r
        }
        else
        {
            let m2 = _l < 0.5 ? _l * (1 + _s) : (_l + _s) - (_l * _s)
            let m1 = 2 * _l - m2
            .r = hueToCol(m1, m2, _h + (1.0 / 3.0))
            .g = hueToCol(m1, m2, _h)
            .b = hueToCol(m1, m2, _h - (1.0 / 3.0))
        }
    }

    // Convert color to HSL values (hue, saturation, lightness in range [0, 1]).
    mtd const toHsl()->{ h: f32, s: f32, l: f32 }
    {
        var result: retval = undefined

        let (rf, gf, bf) = .toRgbf()
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

    // Convert the color to a linear-light Math.Vector4.
    // Components are normalized to [0, 1], with gamma-correct sRGB → linear conversion applied.
    // Useful for rendering, lighting, and blending operations where
    // linear color space is required instead of gamma-encoded sRGB.
    #[ConstExpr]
    mtd const toLinear()->Math.Vector4
    {
        // Convert a normalized sRGB channel [0,1] to linear light.
        func srgbToLinear(c: f32)->f32
        {
            return c <= 0.04045 ? (c / 12.92) : Math.pow((c + 0.055) / 1.055, 2.4)
        }

        var result: retval = undefined
        result.w = .a / 255.0 // alpha channel
        result.x = srgbToLinear(.r / 255.0) // red channel
        result.y = srgbToLinear(.g / 255.0) // green channel
        result.z = srgbToLinear(.b / 255.0) // blue channel
        return result
    }

    // Create a new Color from a linear-light Math.Vector4.
    // Components are expected to be normalized in [0, 1].
    // Performs gamma correction (linear → sRGB) on the RGB channels.
    // Useful for reconstructing an 8-bit Color after doing math in linear space.
    #[ConstExpr]
    func fromLinear(vec: Math.Vector4)->Color
    {
        // Convert a linear-light channel [0,1] to gamma-encoded sRGB.
        func linearToSrgb(c: f32)->f32
        {
            return c <= 0.0031308 ? (c * 12.92) : (1.055 * Math.pow(c, 1.0 / 2.4) - 0.055)
        }

        var result: retval = undefined
        result.a = cast(u8) Math.round(Math.saturate(vec.w) * 255.0)
        result.r = cast(u8) Math.round(Math.saturate(linearToSrgb(vec.x)) * 255.0)
        result.g = cast(u8) Math.round(Math.saturate(linearToSrgb(vec.y)) * 255.0)
        result.b = cast(u8) Math.round(Math.saturate(linearToSrgb(vec.z)) * 255.0)
        return result
    }

    // Calculate the relative luminance (Y) of the color in linear light.
    // Uses the linearized RGB values from toLinear() and BT.709 coefficients (WCAG 2.x).
    // Returns a scalar in [0, 1] representing perceived brightness.
    // Formula:
    //   Y = 0.2126 * R_lin + 0.7152 * G_lin + 0.0722 * B_lin
    // where R_lin, G_lin, B_lin are the linearized channels.
    //#[Inline, ConstExpr]
    mtd const getRelativeLuminance()->f32
    {
        let lin = .toLinear()
        return lin.x * 0.2126 + lin.y * 0.7152 + lin.z * 0.0722
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
        let x1   = cast(f32) .r - col.r
        let x2   = cast(f32) .g - col.g
        let x3   = cast(f32) .b - col.b
        var dist = Math.sqrt((x1 * x1) + (x2 * x2) + (x3 * x3))
        dist /= Math.sqrt(255.0 * 255.0 * 3.0) // Normalize to [0, 1]
        return dist
    }

    // Check if the color is considered "dark" based on luminance.
    #[Inline]
    mtd const isDark()->bool
    {
        return .getGrayScalef() < 0.5
    }

    // Check if the color is considered "light" based on luminance.
    #[Inline]
    mtd const isLight()->bool
    {
        return .getGrayScalef() >= 0.5
    }

    // Get a contrasting color (black or white) for text readability.
    #[Inline, ConstExpr]
    mtd const getContrasting()->Color
    {
        return .isDark() ? Color.fromArgb(Argb.White) : Color.fromArgb(Argb.Black)
    }

    // Calculate the perceived brightness using luminance formula.
    #[Inline, ConstExpr]
    mtd const getGrayScalef()->f32
    {
        return (cast(f32) .r * 0.299 + cast(f32) .g * 0.587 + cast(f32) .b * 0.114) / 255.0
    }

    // Convert to grayscale color preserving alpha.
    #[Inline, ConstExpr]
    mtd const getGrayscale()->Color
    {
        let gray = cast(u8) .getGrayScalef() * 255
        return Color.fromArgb(.a, gray, gray, gray)
    }

    // Calculates contrast ratio between this color and another.
    // Uses the standard WCAG contrast formula: (l1 + 0.05) / (l2 + 0.05)
    mtd const getContrastRatio(against: Color)->f32
    {
        let l1 = Math.max(.getRelativeLuminance(), against.getRelativeLuminance())
        let l2 = Math.min(.getRelativeLuminance(), against.getRelativeLuminance())
        return (l1 + 0.05) / (l2 + 0.05)
    }

    // Adjust saturation (-1.0 = grayscale, 0.0 = no change, 1.0 = full saturation).
    #[ConstExpr]
    mtd const adjustSaturation(factor: f32)->Color
    {
        let hsl    = .toHsl()
        let newS   = Math.clamp(hsl.s + factor, 0.0, 1.0)
        var result = Color.fromHsl(hsl.h, newS, hsl.l)
        result.a = .a // Preserve alpha
        return result
    }

    // Adjust brightness (-1.0 = black, 0.0 = no change, 1.0 = white).
    #[ConstExpr]
    mtd const adjustBrightness(factor: f32)->Color
    {
        let hsl    = .toHsl()
        let newL   = Math.clamp(hsl.l + factor, 0.0, 1.0)
        var result = Color.fromHsl(hsl.h, hsl.s, newL)
        result.a = .a // Preserve alpha
        return result
    }

    // Returns a new color adjusted so that its contrast ratio against `target`
    // meets or exceeds the given `requiredContrast` (WCAG-compatible).
    //
    // The function iteratively darkens or lightens the color depending on
    // which direction increases contrast most effectively.
    //
    // - Parameters:
    //   - target: The color to contrast against (e.g., foreground or background).
    //   - requiredContrast: The minimum contrast ratio (default = 4.5 for normal text).
    //   - step: Brightness adjustment per iteration. Positive values move toward lighter,
    //           negative toward darker; the function automatically picks direction.
    //   - maxSteps: Maximum number of iterations (safety cap).
    // - Returns: A new color meeting or exceeding the required contrast ratio.
    mtd ensureContrast(target: Color, requiredContrast: f32 = 4.5, step: f32 = 0.02, maxSteps: s32 = 64)
    {
        let initialContrast = .getContrastRatio(target)

        // If already meets the contrast requirement, return unchanged
        if initialContrast >= requiredContrast do
            return

        // Decide direction: should we go lighter or darker?
        // Try both directions and see which one increases contrast more.
        let astep             = Math.abs(step)
        let lighter           = .adjustBrightness(astep)
        let darker            = .adjustBrightness(-astep)
        let contrastIfLighter = lighter.getContrastRatio(target)
        let contrastIfDarker  = darker.getContrastRatio(target)

        // Direction that improves contrast more
        let direction: f32 = (contrastIfLighter > contrastIfDarker) ? astep : -astep

        var steps    = 0
        var contrast = initialContrast

        // Iterate until we reach or exceed required contrast
        while contrast < requiredContrast and steps < maxSteps
        {
            dref me  = .adjustBrightness(direction)
            contrast = .getContrastRatio(target)
            steps += 1
        }
    }

    // Adjust hue by rotating around the color wheel.
    #[ConstExpr]
    mtd const rotateHue(degrees: f32)->Color
    {
        let hsl  = .toHsl()
        var newH = hsl.h + (degrees / 360.0)
        if newH > 1.0 do
            newH -= 1.0
        elif newH < 0.0 do
            newH += 1.0
        var result = Color.fromHsl(newH, hsl.s, hsl.l)
        result.a = .a // Preserve alpha
        return result
    }

    // Create a color with modified alpha.
    #[Inline, ConstExpr]
    mtd const withAlpha(alpha: u8)->Color
    {
        return fromArgb(alpha, .r, .g, .b)
    }

    // Create a color with modified alpha (float version).
    #[Inline, ConstExpr]
    mtd const withAlphaf(alpha: f32)->Color
    {
        return fromArgb(cast(u8) (Math.clamp(alpha, 0.0, 1.0) * 255), .r, .g, .b)
    }

    // Get complementary color (opposite on color wheel).
    #[ConstExpr]
    mtd const getComplementary()->Color
    {
        return .rotateHue(180.0)
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

    // Generates N visually distinct colors.
    // Uses golden-angle hue spacing to ensure that each color is clearly separated from the others,
    // with slight lightness variation for good contrast against white text.
    func generateDistinctColors(n: u32, startHue: f32 = 0.113, lum: f32 = 0.44, sat: f32 = 0.68)->Array'Color
    {
        var colors: retval
        if n == 0 do
            return colors

        const G    = 0.61803398875     // Golden ratio conjugate for low-discrepancy hue sampling
        const LVAR = 0.05              // tiny alternation to separate neighbors

        for i in n
        {
            // Evenly jump around the hue circle with the golden angle
            let h = Math.fract(startHue + cast(f32) i * G)

            // Slight alternating lightness to enhance contrast between adjacent colors
            let l = lum + ((i & 1) == 0 ? -LVAR : LVAR)

            var c = fromHsl(h, sat, l)
            c.a = 255
            colors.add(c)
        }

        return colors
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
        if @countof(one) == 0 do
            throw SyntaxError{}

        if buf[0] == '0' and buf[1] | 32 == 'x'
        {
            (.argb, eat, ovf) = try parseU32(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)
        }
        elif Latin1.isDigit(buf[0])
        {
            (.a, eat, ovf) = try parseU8(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (.r, eat, ovf) = try parseU8(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (.g, eat, ovf) = try parseU8(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)

            one = Tokenize.eatSpaces(one)
            (.b, eat, ovf) = try parseU8(one)
            if ovf do
                throw OverflowError{}
            one = Tokenize.eatCount(one, eat)
        }
        else
        {
            let id = try parseIdentifier(one)
            one   = Tokenize.eatCount(one, @countof(id))
            .argb = cast(u32) Argb.fromName(id)
        }

        return one
    }
}
