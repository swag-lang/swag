

struct Point
{
	x, y, z = 4
}

#test
{
	pt := Point{}
	@assert(pt.x + pt.y + pt.z == 12)
}