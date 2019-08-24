#module module637
//#unittest pass semantic

struct(T) Point
{
	var x: s32 = 666
}

//#[swag.printbc]
func toto(v: Point)
{
	@print(v.x)
}

//#[swag.printbc]
#test
{
	var pt: Point!(s32)
	toto(pt)
}
