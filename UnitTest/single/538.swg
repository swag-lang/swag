#module module538
//#unittest pass semantic

struct point1
{
	var x: s32 = 666
	var y: s32
	var z: s32 = 100
}

struct point
{
	var in: point1
	var x: s32 = 10
	var y: s32
}

//#[swag.printbytecode]
#test
{
	var pt: point
	@assert(pt.x == 10)
	@assert(pt.in.x == 666)
	@assert(pt.in.z == 100)
}