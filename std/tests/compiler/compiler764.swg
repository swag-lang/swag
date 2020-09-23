

struct Point
{
	x: s32 = 666
	y: s32
}

//#[swag.printbc]
#test
{
	cpt := Point{}
	@assert(cpt.x == 666)
	@assert(cpt.y == 0)
}
