#module module803

struct Point
{
	x: s32
}

//#unittest error
//#[swag.printbc]
func toto(using pt: *Point)
{
	x = 10
}

//#[swag.printbc]
#test
{
	var pt: Point
	toto(pt)
	@assert(pt.x == 10)
}