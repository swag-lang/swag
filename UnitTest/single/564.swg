#module module564
//#unittest pass semantic

//#[swag.printbc]
struct point
{
	var x: s32
	var y: s32
	var z: s32
}

#test
{
	var pt : point
	@assert(pt.x == 0)
	pt.x = 10
	@assert(pt.x == 10)
	pt.opInit()
	@assert(pt.x == 0)
}