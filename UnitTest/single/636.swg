#module module636
//#unittest pass semantic

struct(T) Point
{
}

//#[swag.printbc]
func toto(x: Point)
{
}

#test
{
	var x: Point!(s32)
	//toto(x)
}
