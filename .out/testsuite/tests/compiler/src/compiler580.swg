


struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opEquals(self, other: s32)->bool
	{
		return self.x == other and self.y == other
	}
}

#test
{
	var pt: Point
	@assert(pt != 1)
	pt.x = 0; pt.y = 1;
	@assert(pt != 1)
	pt.x = 1;
	@assert(pt == 1)
}