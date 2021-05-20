


struct point1
{
	x: s32
	y: s32
	z: s32
}

struct point
{
	in: point1
	x: s32
	y: s32
}

//#[Swag.printbc]
#test
{
	var pt: point
	pt.in.x = 100
	@assert(pt.in.x == 100)
}