


struct point
{
	x: s32 = 10
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var pt: point
	@assert(pt.x == 10)
}