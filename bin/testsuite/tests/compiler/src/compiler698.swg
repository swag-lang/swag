


struct Point
{
	x: s32 = 5
	y: s32 = 10
}


//#[Swag.PrintBc]
#test
{
	var pt: Point
	let ptr = &pt
	@assert(ptr.x == 5)
	@assert(ptr.y == 10)
}