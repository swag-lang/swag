

struct Point
{
	x: s32 = 666
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	var slice: *Point = &pt
	@assert(slice[0].x == 666)
}