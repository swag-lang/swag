

struct Point
{
	x: s32
	y: s32
}

//#[Swag.printbc]
#test
{
	var pt: Point
	using pt
	x = 10
	y = 20
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}