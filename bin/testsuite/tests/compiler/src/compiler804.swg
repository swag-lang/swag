

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
func toto(using pt: *Point)
{
	x = 10
	y = 20
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	toto(&pt)
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}