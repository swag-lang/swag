#global testerror

struct Point
{
	x: s32
	y: s32
}

const nullPoint: Point{10, 50}

//#[swag.printbc]
#test
{
	nullPoint.x = 10
}
