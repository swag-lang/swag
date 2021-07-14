


struct Point
{
	x: s32 = 5
	y: s32 = 10
}

struct Array
{
	pt: Point
}

//#[Swag.PrintBc]
#test
{
	var arr: Array
	ptr := &arr.pt
	@assert(ptr.x == 5)
	@assert(ptr.y == 10)
}