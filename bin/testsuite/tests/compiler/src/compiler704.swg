#global testerror "Err0082"

struct(T) Point
{
	x: T = 5
	y: T = 100
}

/*struct Array
{
	var pt1: Point
}*/

//#[Swag.PrintBc]
#test
{
	var pt: Point
}