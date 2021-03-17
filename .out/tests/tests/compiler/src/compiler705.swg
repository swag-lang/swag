#global testerror

struct(T) Point
{
	x: T = 5
	y: T = 100
}

struct Array
{
	pt1: Point
}

//#[swag.printbc]
#test
{
	var pt: Array
}