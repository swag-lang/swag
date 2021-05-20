


struct Point
{
	x: s32 = 5
	y: s32 = 10
}

//#[Swag.printbc]
#test
{
	var pt : [5] Point{5, 10}
	@assert(pt[4].x == 5)
	@assert(pt[4].y == 10)
}