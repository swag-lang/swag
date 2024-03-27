


struct Point
{
	x: s32
	y: s32
}

#test
{
	var a, b : Point
	b.x = 10
	b.y = 100
	a = b
	@assert(a.x == 10)
	@assert(a.y == 100)
}