

struct Point
{
	x: s32
	y: s32
}

var pt: Point

//#[Swag.printbc]
#test
{
	using pt
	x = 10
	y = 20
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}