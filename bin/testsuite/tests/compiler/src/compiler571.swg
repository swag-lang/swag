//#global testerror "Err0315"

struct Point
{
	x: s32
	y: s32
}

impl Point
{
	/*func opEqual(self, other: Point)->bool
	{
		return self.x == other.x and self.y == other.y
	}*/
}

//#[Swag.PrintBc]
#test
{
	var pt1 : Point
	var pt2 : Point
	@assert(pt1 == pt2)
	pt1.x = 1
	@assert(pt1 != pt2)
}