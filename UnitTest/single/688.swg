#module module688
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 10
}

//#[swag.printbc]
#test
{
	var pt : Point!s32(100, 500)
	@assert(pt.x == 100)
	@assert(pt.y == 500)
}