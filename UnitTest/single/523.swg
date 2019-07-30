#module module523
//#unittest pass semantic

struct point
{
	var x: s32
	var y: s32
	var z: s32
}

#[swag.printbytecode]
#test
{
	var pt: point
	@assert(pt.x == 0)
	@assert(pt.y == 0)
	@assert(pt.z == 0)
}
