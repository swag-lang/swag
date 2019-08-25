#module module645
//#unittest pass semantic

struct(T) Point
{
	var x: T = 6
}

//#[swag.printbc]
#test
{
	var pt: Point!s32
	pt.opInit()
}
