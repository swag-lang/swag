


struct Point
{
	x: s32 = 5
	y: s32 = 10
}

//#[swag.printbc]
#test
{
	var pt : [2, 2] Point{100, 500}
	@assert(pt[1, 1].x == 100)
	@assert(pt[1, 1].y == 500)
}