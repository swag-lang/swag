namespace std.drawing
{
	public struct Color
	{
		a: u8
		r: u8
		g: u8
		b: u8
	}

	impl Color
	{
		public
		{
			func opAffect(using self, argb: u32)
			{
				a = acast (argb >> 24)
				r = acast ((argb >> 16) & 0xFF)
				g = acast ((argb >> 8) & 0xFF)
				b = acast (argb & 0xFF)
			}
		}
	}
}