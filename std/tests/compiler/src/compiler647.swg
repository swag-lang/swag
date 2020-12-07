#module "compiler647"


struct(T) Point
{
	x: T = 6
}

//#[swag.printbc]
#unittest error
#test
{
	var pt: Point!bool
}
