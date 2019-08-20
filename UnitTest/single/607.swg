#module module607
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
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