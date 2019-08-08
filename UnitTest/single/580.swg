#module module580
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

impl Point
{
	func opEquals(self, other: s32)->bool
	{
		return self.x == other && self.y == other
	}
}

#test
{
	var pt: Point
	@assert(pt == 0)
}