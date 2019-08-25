#module module647
//#unittest pass semantic

struct(T) Point
{
	var x: T = 6
}

//#[swag.printbc]
#unittest error
#test
{
	var pt: Point!bool
}
