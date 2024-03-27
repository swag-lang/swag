


struct(T) Point
{
	x: T = 1
}

impl Point
{
	//#[Swag.PrintBc]
	func toto(xx: T)->T
	{
		return xx
	}
}

//#[Swag.PrintBc]
#test
{
	var pt: Point's32
	@assert(Point.toto(10) == 10)
	@assert(Point.toto(10.0) == 10.0)
}
