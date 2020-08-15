

struct Point
{
	x: s32 = 10
	y: s32 = 20
}

//#[swag.printbc]
#test
{
	var pt : Point
	pt.x = 0
	pt.y = 0
	ptr := &pt
	@init(ptr)
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}