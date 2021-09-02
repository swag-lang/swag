

struct Point
{
	x: s32 = 666
	y: s32
}

#test
{
	cpt := Point{}
	@assert(cpt.x == 666)
	@assert(cpt.y == 0)
}
