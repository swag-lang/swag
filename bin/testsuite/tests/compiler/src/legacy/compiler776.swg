/*

struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opPostMove(me)
	{
		me.x = 0
		me.y = 0
	}
}

//#[Swag.PrintBc]
#test
{
	var pt1: Point(10, 20)
	var pt2: Point

	@assert(pt1.x == 10)
	@assert(pt1.y == 20)
	pt2 = #move pt1
	@assert(pt2.x == 10)
	@assert(pt2.y == 20)
	@assert(pt1.x == 0)
	@assert(pt1.y == 0)
}*/
