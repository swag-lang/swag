

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	var pt1: Point
	var pt2: Point{10, 20}
	pt1 = pt2
	@assert(pt1.x == 10)
	@assert(pt1.y == 20)
}
