#global testerror "Err0066"

struct(T) Point
{
	x: T = 5
	y: T = 10
}

typealias Point2f = Point

//#[Swag.PrintBc]
#test
{
	var pt : Point2f{1.0, 2.0}
}