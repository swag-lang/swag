


struct point1
{
	x: s32
	y: s32
	z: s32 = 1
}

//#[Swag.PrintBc]
struct point
{
	pt: point1
	pt1: point1
	pt2: point1
}

//#[Swag.PrintBc]
#test
{
	var pt : point
	@assert(pt.pt.z == 1)
	@assert(pt.pt1.z == 1)
	@assert(pt.pt2.z == 1)
}