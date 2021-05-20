


struct(T) Point
{
	x: s32
}

//#[Swag.printbc]
func toto(x: Point'(s32)){}
func toto1(x: Point'(s32)){}
func toto2(x: Point'(s32)){}

#test
{
	var x: Point'(s32)
	toto(x)
}
