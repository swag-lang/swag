struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	var pt : [5] Point{10, 50}
	@assert(pt[4].x == 10)
	@assert(pt[4].y == 50)
}