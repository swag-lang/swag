

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#unittest error
func toto(using pt: u32)
{
	x = 10
	y = 20
}
