


#global testerror
struct Point
{
	x: s32 = 10
	y: s32 = 20
}

var pt: 	Point = {1, 2}
var pt1: 	Point = {1, pt.x}
