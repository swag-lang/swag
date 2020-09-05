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

	impl Color
	{
        #[swag.inline]
		public
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

			func opEquals(self, _argb: u32) => self.argb == _argb
            func opEquals(self, other: Self) => self.argb == other.argb
		}
	}
}