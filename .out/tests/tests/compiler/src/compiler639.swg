


struct(T) Point
{
	x: T = 1
}

//#[Swag.printbc]
func toto(v: Point)
{
}

//#[Swag.printbc]
#test
{
	var pt: Point'(s32)
	toto(pt)
	@assert(pt.x == 1)
	var pt1: Point'(f32)
	@assert(pt1.x == 1.0)
}
