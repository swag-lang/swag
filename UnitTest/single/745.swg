#module module745

struct Point1
{
	var x: s32
}

struct Point
{
	var x: s32 = ?
	var y: s32 = ?
	var pt: Point1
}

//#[swag.printbc]
#test
{
	var pt: Point
}