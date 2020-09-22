


struct Point
{
	x: s32 = 1
	y: s32 = 2
}

var x: Point{10,20}

//#[swag.printbc]
#test
{
	@assert(x.x == 10)
	@assert(x.y == 20)
}
