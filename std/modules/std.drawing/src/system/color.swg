namespace std.drawing
{
	public struct Color
	{
		#if std.BIG_ENDIAN
		{
			a, r, g, b: u8
			#[swag.offset("a")]
			argb: u32
		}
		#else
		{
			b, g, r, a: u8
			#[swag.offset("b")]
			argb: u32
		}
	}

	public impl Color
	{
        #[swag.inline]
		{
            #[swag.complete]
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

            // Just set r, g and b components.
            // Alpha will be opaque
            func setRgb(self, r, g, b: u8)
            {
                self.a = 255
                self.r = r
                self.g = g
                self.b = b
            }

            // Retreive all components as floating point values
            // between 0 and 1.
            func toFloat(self) -> {a: f32, r: f32, g: f32, b: f32}
            {
                _a := self.a / 255.0'f32
                _r := self.r / 255.0'f32
                _g := self.g / 255.0'f32
                _b := self.b / 255.0'f32
                return @{_a, _r, _g, _b}
            }
		}
	}
}