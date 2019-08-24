#module module638
//#unittest pass semantic

struct(T) Point
{
	var x: T = 1
}

//#[swag.printbc]
func toto(v: Point)
{
}

//#[swag.printbc]
#test
{
	var pt: Point!(s32)
	toto(pt)
	var pt1: Point!(f32)
	toto(pt1)
}
