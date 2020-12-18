


struct(T) Point
{
	x: s32 = 666
}

//#[swag.printbc]
func toto(v: Point)
{
	//@assert(v.x == 666)
}

//#[swag.printbc]
#test
{
	var pt: Point'(s32)
	toto(pt)
}
