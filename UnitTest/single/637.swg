#module module637
//#unittest pass semantic

struct(T) Point
{
	var x: s32 = 666
}

//#[swag.printbc]
func toto(x: Point)
{
	//@print(x.x)
}

#test
{
	var x: Point!(s32)
	toto(x)
}
