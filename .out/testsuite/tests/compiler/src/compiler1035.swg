

struct Point
{
	x, y: s32
}
//#[Swag.PrintBc]
#test
{
	var pt : Point
	ptr := &pt
	@init(ptr)(10, 20)
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}