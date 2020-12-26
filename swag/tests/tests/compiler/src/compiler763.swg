

struct Point
{
	x: s32 = 666
	y: s32
}

//#[swag.printbc]
#test
{
	cpt := Point{10, 20}
	@assert(cpt.x == 10)
	@assert(cpt.y == 20)
}
