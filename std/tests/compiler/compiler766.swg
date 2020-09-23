

struct Point
{
	x: s32 = 666
	y: s32
}

//#[swag.printbc]
func toto()->Point
{
	return Point{10, 20}
}

//#[swag.printbc]
#test
{
	var cpt : Point = toto()
	@assert(cpt.x == 10)
	@assert(cpt.y == 20)
}
