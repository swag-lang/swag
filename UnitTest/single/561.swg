#module module561
#unittest pass semantic

//#[swag.printbytecode]
#test
{
	var pt: point
	pt.x = 100
	@assert(pt.x == 100)
	@assert(pt.y == 70)
	@assert(pt.x == 100 && pt.y == 70)
	pt.opInit()
	@assert(pt.x == 60)
	@assert(pt.y == 70)
}

struct point
{
	var x: s32 = 60
	var y: s32 = 70
}