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
			func opAffect(self, _argb: u32)
            {
                self.argb = _argb
            }

            #[swag.complete]
            func opAffect(self, _argb: Argb)
            {
                self.argb = cast(u32) _argb
            }

            #[swag.complete]
            func opAffect(using self, name: string)
            {
                argb = cast(u32) Argb.fromName(name)
            }

			func opEquals(self, _argb: u32) => self.argb == _argb
            func opEquals(self, _argb: Argb) => self.argb == cast(u32) _argb
            func opEquals(self, other: Self) => self.argb == other.argb

            func setArgb(self, _a, _r, _g, _b: u8)
            {
                self.a = _a
                self.r = _r
                self.g = _g
                self.b = _b
            }

            func setRgb(self, _r, _g, _b: u8)
            {
                self.a = 255
                self.r = _r
                self.g = _g
                self.b = _b
            }
		}
	}
}