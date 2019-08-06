#module module562
//#unittest pass semantic

#[swag.printbytecode]
#test
{
	var pt: point
	@assert(pt.y == 70)
}

struct point
{
	var x: s32 = 60
	var y: s32 = 70
}