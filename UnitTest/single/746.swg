#module module746

struct Point
{
	var x: s32 = ?
	var y: s32 = ?
}

//#[swag.printbc]
#test
{
	var pt: Point
	pt.opInit()
}