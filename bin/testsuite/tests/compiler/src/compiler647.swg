#global testerror "Err0643"

struct(T) Point
{
	x: T = 6
}

//#[Swag.PrintBc]
#test
{
	var pt: Point'bool
}
