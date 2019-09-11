#module module803

struct Point
{
	x: s32
}

//#unittest error
func toto(using pt: Point)
{
	x = 0
}

//#[swag.printbc]
#test
{
}