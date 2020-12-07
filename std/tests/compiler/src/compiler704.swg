#module "compiler704"


struct(T) Point
{
	x: T = 5
	y: T = 100
}

/*struct Array
{
	var pt1: Point
}*/

//#[swag.printbc]
#unittest error
#test
{
	var pt: Point
}