

struct Point
{
	x, y: s32
}

//#[Swag.PrintBc]
#test
{
	var pt : [5] Point
	ptr := &pt
	@init(ptr, 5)(10, 20)
	@assert(pt[4].x == 10)
	@assert(pt[4].y == 20)
}