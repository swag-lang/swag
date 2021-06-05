//#[Swag.printbc]
#test
{
	var pt: point
	pt.x = 100
	@assert(pt.x == 100)
	@assert(pt.y == 70)
	@assert(pt.x == 100 and pt.y == 70)
	/*pt.opInit()
	@assert(pt.x == 60)
	@assert(pt.y == 70)*/
}

struct point
{
	x: s32 = 60
	y: s32 = 70
}