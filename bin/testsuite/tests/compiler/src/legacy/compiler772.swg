#global testerror "Err0253"

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var pt1: Point
	pt1 = 1
	@assert(pt1.x == 1)
	@assert(pt1.y == 2)
}

impl Point
{
	func opAffect(me, other: Point)
	{
		//me.x = other
		//me.y = other + 1
	}
}
