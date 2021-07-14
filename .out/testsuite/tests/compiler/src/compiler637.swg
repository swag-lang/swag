


struct(T) Point
{
	x: s32 = 666
}

//#[Swag.PrintBc]
func toto(v: Point)
{
	//@assert(v.x == 666)
}

//#[Swag.PrintBc]
#test
{
	var pt: Point'(s32)
	toto(pt)
}
