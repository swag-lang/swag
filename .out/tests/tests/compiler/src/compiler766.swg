

struct Point
{
	x: s32 = 666
	y: s32
}

//#[Swag.printbc]
func toto()->Point
{
	return Point{10, 20}
}

//#[Swag.printbc]
#test
{
	var cpt : Point = toto()
	@assert(cpt.x == 10)
	@assert(cpt.y == 20)
}
