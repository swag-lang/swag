#module module522
//#unittest pass semantic

//#[swag.printbc]
struct point
{
	x, y: s32
}

#test
{
	var pt: point
	pt.x = 10
	pt.y = 20
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}
