namespace std.math
{
	public struct Point
	{
		x, y: s32
	}

	impl Point
	{
		public
		{
			func clear(using self)
			{
				x, y = 0
			}
		}
	}
}