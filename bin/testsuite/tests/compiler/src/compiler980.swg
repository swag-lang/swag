#global testerror "Err0093"

struct Point
{
	x: s32
	y: s32
}

const NullPoint: Point{10, 50}

//#[Swag.PrintBc]
#test
{
	NullPoint.x = 10
}
