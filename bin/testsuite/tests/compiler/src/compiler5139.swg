#global testerror "Err0672"

struct(T) Point
{
	x: T = 5
	y: T = 100
}

struct(B) Array
{
	pt1: Point
}

//#[Swag.PrintBc]
#test
{
	var pt: Array's32
}