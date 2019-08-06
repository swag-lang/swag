#module module562
//#unittest pass semantic

//#[swag.printbc]
#test
{
	var pt: point
	@assert(pt.y == 70)
	pt.y = 100
	@assert(pt.y == 100)
	pt.opInit()
	@assert(pt.y == 70)
}

struct point
{
	var x: s32 = 60
	var y: s32 = 70
}