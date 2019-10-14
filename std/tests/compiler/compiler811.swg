

struct Point
{
	x: s32
	y: s32
}

#test
{
	if true
	{
		var pt: Point
		using pt
		x = 10
		@assert(x == 10)
		@assert(pt.x == 10)
	}
}

