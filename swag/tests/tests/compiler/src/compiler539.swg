


struct point
{
	x: s32 = 10
	y: s32
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(pt.x == 10)
}