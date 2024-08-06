#global testerror "Err0630"

struct(T) Point
{
	x: T = 6
}

//#[Swag.PrintBc]
#test
{
	var pt: Point'bool
}
