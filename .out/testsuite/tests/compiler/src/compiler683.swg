


struct Point
{
	x: s32 = 100
	y: s32 = 100
}

//#[Swag.PrintBc]
#test
{
	var pt : [5] Point{10, 50}
	@assert(pt[0].x == 10)
	@assert(pt[0].y == 50)
	@assert(pt[4].x == 10)
	@assert(pt[4].y == 50)
}