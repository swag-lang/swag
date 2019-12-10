namespace std.math
{
	public struct Point
	{
		x: f32
        y: f32
	}

	impl Point
	{
		public
		{
			func clear(using self)
			{
				x, y = 0
			}

			func(op: string) opBinary(using self, other: Self)->Self
			{
				#if op == "-"
					return Point(x - other.x, y - other.y)
				#else
					#assert(false, "unsupported operator " ~op)
			}
		}
	}
}
