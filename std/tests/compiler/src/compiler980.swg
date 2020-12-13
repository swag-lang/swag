


struct Point
{
	x: s32
	y: s32
}

const nullPoint: Point{10, 50}

//#[swag.printbc]
#unittest error
#test
{
	nullPoint.x = 10
}
