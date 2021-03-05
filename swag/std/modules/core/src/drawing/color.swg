#global namespace drawing
#global public
using swag

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
    #[inline]
    {
        #[complete]
        {
        func opAffect(self, argb: u32)    = self.argb = argb
        func opAffect(self, argb: Argb)   = self.argb = cast(u32) argb
        func opAffect(self, name: string) = self.argb = cast(u32) Argb.fromName(name)
        }

        func opEquals(self, argb: u32) =>       self.argb == argb
        func opEquals(self, argb: Argb) =>      self.argb == cast(u32) argb
        func opEquals(self, other: Self) =>     self.argb == other.argb

        // Set all components
        func setArgb(self, a, r, g, b: u8)
        {
            self.a = a
            self.r = r
            self.g = g
            self.b = b
        }

        // Set all components with floats in the range [0 1]
        func setArgb(self, a, r, g, b: f32)
        {
            self.a = autocast (math.clamp(a, 0, 1) * 255)
            self.r = autocast (math.clamp(r, 0, 1) * 255)
            self.g = autocast (math.clamp(g, 0, 1) * 255)
            self.b = autocast (math.clamp(b, 0, 1) * 255)
        }

        // Just set r, g and b components.
        // Alpha will be opaque
        func setRgb(self, r, g, b: u8)
        {
            self.a = 255
            self.r = r
            self.g = g
            self.b = b
        }

        // Set r,g,b with floats in the range [0 1]
        // Alpha will be opaque
        func setRgb(self, r, g, b: f32)
        {
            self.a = 255
            self.r = autocast (math.clamp(r, 0, 1) * 255)
            self.g = autocast (math.clamp(g, 0, 1) * 255)
            self.b = autocast (math.clamp(b, 0, 1) * 255)
        }

        // Retreive all components as floating point values between 0 and 1
        func toArgbf(self) -> {a: f32, r: f32, g: f32, b: f32}
        {
            var result: retval = ?
            result.a = self.a / 255.0'f32
            result.r = self.r / 255.0'f32
            result.g = self.g / 255.0'f32
            result.b = self.b / 255.0'f32
            return result
        }

        // Retreive all components as floating point values between 0 and 1
        func toRgbf(self) -> {r: f32, g: f32, b: f32}
        {
            var result: retval = ?
            result.r = self.r / 255.0'f32
            result.g = self.g / 255.0'f32
            result.b = self.b / 255.0'f32
            return result
        }
    }

    // Returns hue, saturation, lightness
    func getHSL(using self) -> {h: f32, s: f32, l: f32}
    {
        var result: retval = ?

        (rf, gf, bf) := toRgbf()
        max := math.max(rf, gf, bf)
        min := math.min(rf, gf, bf)
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
}
