


//#[Swag.printbc]
struct point
{
	x, y: s32
}

#test
{
	var pt: point
	pt.x = 10
	@assert(pt.x == 10)
}
