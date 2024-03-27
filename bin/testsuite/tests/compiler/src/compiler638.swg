


struct(T) Point
{
	x: T = 1
}

//#[Swag.PrintBc]
func toto(v: Point)
{
}

//#[Swag.PrintBc]
#test
{
	var pt: Point'(s32)
	toto(pt)
	var pt1: Point'(f32)
	toto(pt1)
}
