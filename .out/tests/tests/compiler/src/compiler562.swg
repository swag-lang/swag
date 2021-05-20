


//#[Swag.printbc]
#test
{
	var pt: point
	@assert(pt.y == 70)
	pt.y = 100
	@assert(pt.y == 100)
	/*pt.opInit()
	@assert(pt.y == 70)*/
}

struct point
{
	x: s32 = 60
	y: s32 = 70
}