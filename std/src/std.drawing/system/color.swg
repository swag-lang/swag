namespace std.drawing
{
	public struct Color
	{
		#if BIG_ENDIAN
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
			func opAffect(self, argb: u32)
			{
				self.argb = argb
			}

			func opEquals(self, argb: u32) => self.argb == argb
            func opEquals(self, b: Self) => self.argb == b.argb
		}
	}
}