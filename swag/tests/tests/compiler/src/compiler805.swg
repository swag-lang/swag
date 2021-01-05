#global testerror

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
func toto(using pt: **Point)
{
	x = 10
	y = 20
}
