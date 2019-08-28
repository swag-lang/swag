#module module687
//#unittest pass syntax

struct Point
{
	var x: s32 = 5
	var y: s32 = 10
}

//#[swag.printbc]
#test
{
	var pt : [2, 2] Point(100, 500)
	@assert(pt[1, 1].x == 100)
	@assert(pt[1, 1].y == 500)
}