#global namespace drawing
using swag

public struct Color
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

public impl Color
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

        // Retreive all components as floating point values between 0 and 1.
        func toFloat(self) -> {a: f32, r: f32, g: f32, b: f32}
        {
            var result: retval = ?
            result.a = self.a / 255.0'f32
            result.r = self.r / 255.0'f32
            result.g = self.g / 255.0'f32
            result.b = self.b / 255.0'f32
            return result
        }
    }
}
