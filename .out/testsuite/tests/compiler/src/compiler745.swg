

struct Point1
{
	x: s32
}

struct Point
{
	x: s32 = ?
	y: s32 = ?
	pt: Point1
}

//#[Swag.printbc]
#test
{
	var pt: Point
}