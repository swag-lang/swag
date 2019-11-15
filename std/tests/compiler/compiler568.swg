struct point1
{
	x: s32
	y: s32
	z: s32
}

//#[swag.printbc]
struct point
{
	x: s32
	y: s32
	z: s32
	ptx: point1
}

//#[swag.printbc]
#test
{
	var pt : point
	@assert(pt.ptx.x == 0)
	pt.ptx.x = 10
	//@print(pt.ptx.x)
	@assert(pt.ptx.x == 10)
	/*pt.ptx.opInit()
	@assert(pt.ptx.x == 0)*/
}