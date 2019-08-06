#module module539
//#unittest pass semantic

struct point
{
	var x: s32 = 10
	var y: s32
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(pt.x == 10)
}