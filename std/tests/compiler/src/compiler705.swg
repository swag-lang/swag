#module "compiler705"


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
#unittest error
#test
{
	var pt: Array
}