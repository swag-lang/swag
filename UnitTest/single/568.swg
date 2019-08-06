#module module568
//#unittest pass semantic

struct point1
{
	var x: s32
	var y: s32
	var z: s32
}

//#[swag.printbc]
struct point
{
	var x: s32
	var y: s32
	var z: s32
	var ptx: point1
}

//#[swag.printbc]
#test
{
	var pt : point
	@assert(pt.ptx.x == 0)
	pt.ptx.x = 10
	//@print(pt.ptx.x)
	@assert(pt.ptx.x == 10)
	pt.ptx.opInit()
	@assert(pt.ptx.x == 0)
}