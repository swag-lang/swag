#global testerror

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var pt1 : Point
	var pt2 : Point
	pt1.x = 1; pt2.x = 1;
	@assert(pt1 == pt2)
	pt2.x = 2;
	@assert(pt1 != pt2)
}

#global testerror
impl Point
{
	func opEqual(self, other: Point) -> bool
	{
		return self.x == other.x and self.y == other.y
	}
}