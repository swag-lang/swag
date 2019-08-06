#module module521
#unittest pass semantic

//#[swag.printbc]
struct point
{
	var x, y: s32
}

#test
{
	var pt: point
	pt.x = 10
	@assert(pt.x == 10)
}
