


struct Point
{
	x: s32
	y: s32
}

#unittest error
//#[swag.printbc]
#test
{
	var pt1 : Point
	var pt2 : Point
	pt1.x = 1; pt2.x = 1;
	@assert(pt1 == pt2)
	pt2.x = 2;
	@assert(pt1 != pt2)
}

#unittest error
impl Point
{
	func opEqual(self, other: Point) -> bool
	{
		return self.x == other.x && self.y == other.y
	}
}