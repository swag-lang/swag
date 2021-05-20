


struct(T) Point
{
	x: s32 = 666
}

//#[Swag.printbc]
func toto(v: Point)
{
	//@assert(v.x == 666)
}

//#[Swag.printbc]
#test
{
	var pt: Point'(s32)
	toto(pt)
}
