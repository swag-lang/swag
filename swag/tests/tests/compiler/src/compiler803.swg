#global testerror

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
func toto(using pt: Point)
{
	x = 10
	y = 20
}

//#[swag.printbc]
#test
{
	/*var pt: Point
	toto(pt)
	@assert(pt.x == 10)
	@assert(pt.y == 20)*/
}