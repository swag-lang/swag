


struct Point
{
	x: s32
	y: s32
}

const nullPoint: Point{10, 50}

//#[swag.printbc]
#test
{
	@assert(nullPoint.x == 10)
	@assert(nullPoint.y == 50)
	//nullPoint.x = 10
}
