#module module685
//#unittest pass syntax

struct Point
{
	var x: s32 = 5
	var y: s32 = 10
}

//#[swag.printbc]
#test
{
	var pt : [5] Point(5, 10)
	@assert(pt[4].x == 5)
	@assert(pt[4].y == 10)
}