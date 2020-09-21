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

            /*func extract(self) -> {a: f32, r: f32, g: f32, b: f32}
            {
                a := self.a / 255.0'f32
                r := self.r / 255.0'f32
                g := self.g / 255.0'f32
                b := self.b / 255.0'f32
                return @{a, r, g, b}
            }*/
		}
	}
}