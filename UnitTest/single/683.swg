#module module683
//#unittest pass syntax

struct Point
{
	var x: s32 = 100
	var y: s32 = 100
}

#[swag.printbc]
#test
{
	var pt : [5] Point(10, 50)
	@assert(pt[0].x == 10)
	@assert(pt[0].y == 50)
	@assert(pt[4].x == 10)
	@assert(pt[4].y == 50)
}