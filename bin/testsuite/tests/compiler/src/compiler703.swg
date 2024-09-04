#global testerror "Err0518"

struct(T) Point
{
	x: T = 5
	y: T = 100
}

struct(T, V) Array
{
	pt1: Point'T
	pt2: Point'V
}

//#[Swag.PrintBc]
#test
{
	var arr: Array'(s32, bool)
}