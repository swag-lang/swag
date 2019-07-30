#module module524
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
	var pt: [10] s32
	@assert(pt[0] == 0)
	@assert(pt[9] == 0)
}
