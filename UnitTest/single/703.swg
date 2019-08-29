#module module703
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 100
}

struct(T, V) Array
{
	var pt1: Point!T
	var pt2: Point!V
}

//#[swag.printbc]
#unittest error
#test
{
	var arr: Array!(s32, bool)
}