#module module640
//#unittest pass semantic

struct(T) Point
{
	var x: T = 1
}

//#[swag.printbc]
#test
{
	var pt: Point!s32
	@assert(pt.x == 1)
}
