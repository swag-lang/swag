#global testerror

struct(T) Point
{
	x: T = 5
	y: T = 10
}

alias Point2f = Point

//#[Swag.printbc]
#test
{
	var pt : Point2f(1.0, 2.0)
}