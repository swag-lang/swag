
#global testerror

struct Point
{
	x: s32
	y: s32
}

const nullPoint: Point(10, 50) = {100, 500}

//#[swag.printbc]
#test
{
	@assert(nullPoint.x == 100)
	@assert(nullPoint.y == 500)
}
