struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var pt : [5] Point{10, 50}
	@assert(pt[4].x == 10)
	@assert(pt[4].y == 50)
}