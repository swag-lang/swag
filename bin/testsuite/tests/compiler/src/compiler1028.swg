

struct Point
{
	x: s32 = 10
}

//#[Swag.PrintBc]
#test
{
	var pt : [2] Point
	pt[0].x = 0
	pt[1].x = 0
	ptr := &pt
	@init(ptr, 2)
	@assert(pt[0].x == 10)
	@assert(pt[1].x == 10)
}