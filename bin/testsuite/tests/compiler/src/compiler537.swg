


struct point1
{
	x: s32
	y: s32
	z: s32
}

struct point
{
	inT: point1
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var pt: point
	pt.inT.x = 100
	@assert(pt.inT.x == 100)
}