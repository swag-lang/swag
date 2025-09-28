struct point1
{
	x: s32
	y: s32
	z: s32
}

//#[Swag.PrintBc]
struct point
{
	x: s32
	y: s32
	z: s32
	pt: point1
}

//#[Swag.PrintBc]
#test
{
	var pt : point
	//point1.opInit(pt.pt)
	/*@assert(pt.pt.x == 0)
	pt.pt.x = 10
	@print(pt.pt.x)
	@assert(pt.pt.x == 10)
	pt.pt.opInit()
	@assert(pt.pt.x == 0)*/
}