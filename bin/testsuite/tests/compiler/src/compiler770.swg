/*
struct Point
{
	x: s32
	y: s32
}

var cpt: s32 = 0

impl Point
{
	/*func opAffect(me, other: me)
	{
		me.x = other.x + 1
		me.y = other.y + 1
	}*/

	func opPostMove(me)
	{
		@assert(me.x == 1)
		cpt += 1
	}
}

//#[Swag.PrintBc]
#test
{
	cpt = 0
	var pt1: Point
	pt1	= Point(1, 2)
	@assert(cpt == 1)
}
*/
