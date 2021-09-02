#global testerror

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

#[Swag.PrintBc]
#test
{
	var pt1 : Point
	var pt2 : Point
	pt1.x = 1; pt1.x = 2;
	@assert(pt1 == pt2)
}