#module module537
#unittest pass semantic

struct point1
{
	var x: s32
	var y: s32
	var z: s32
}

struct point
{
	var in: point1
	var x: s32
	var y: s32
}

//#[swag.printbytecode]
#test
{
	var pt: point
	pt.in.x = 100
	@assert(pt.in.x == 100)
}