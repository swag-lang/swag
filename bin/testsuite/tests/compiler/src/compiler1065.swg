

struct Point
{
	x = 4
}

#test
{
	pt := Point{}
	@assert(pt.x == 4)
}