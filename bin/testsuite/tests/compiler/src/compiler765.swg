

struct Point
{
	x: s32 = 666
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var cpt : Point
	@assert(cpt.x == 666)
	@assert(cpt.y == 0)

	cpt = Point{1, 2}
	@assert(cpt.x == 1)
	@assert(cpt.y == 2)
}
