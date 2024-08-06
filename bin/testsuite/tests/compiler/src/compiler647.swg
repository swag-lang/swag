#global testerror "Err0599"

struct(T) Point
{
	x: T = 6
}

//#[Swag.PrintBc]
#test
{
	var pt: Point'bool
}
