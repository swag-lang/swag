#module module705
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 100
}

struct Array
{
	var pt1: Point
}

//#[swag.printbc]
#unittest error
#test
{
	var pt: Array
}