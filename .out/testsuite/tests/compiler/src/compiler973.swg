struct Point
{
	x: s32 = 10
	y: s32 = 20
}

var pt: 	Point = @{1, 2}
var pt1: 	Point = @{1, 20}

//#[Swag.printbc]
#test
{
	@assert(pt.x == 1)
	@assert(pt.y == 2)
	@assert(pt1.x == 1)
	@assert(pt1.y == 20)
}
