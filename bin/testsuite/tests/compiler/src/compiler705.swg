#global testerror "Err0503"

struct(T) Point
{
	x: T = 5
	y: T = 100
}

struct Array
{
	pt1: Point
}

//#[Swag.PrintBc]
#test
{
	var pt: Array
}