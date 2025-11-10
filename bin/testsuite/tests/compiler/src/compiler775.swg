/*
struct Point
{
	x: s32
	y: s32
}

var cpt: s32 = 0

impl Point
{
	func opPostCopy(me)
	{
		@assert(false)
	}

	func opPostMove(me)
	{
		cpt += 1
	}

	//#[Swag.PrintBc]
	func(op: string) opBinary(me, other: me)->Point
	{
		return Point(me.x + other.x, me.y + other.y) // #move
	}
}

//#[Swag.PrintBc]
#test
{
	cpt = 0

	var pt1: Point
	var pt2: Point

	var pt3: Point
	pt3 = pt1 + pt2 // #move
	@assert(cpt == 2)
}
*/
