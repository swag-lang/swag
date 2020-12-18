


struct(T) Point
{
	x: T = 1
}

impl Point
{
	//#[swag.printbc]
	func toto(xx: T)->T
	{
		return xx
	}
}

//#[swag.printbc]
#test
{
	var pt: Point's32
	@assert(Point.toto(10) == 10)
	@assert(Point.toto(10.0) == 10.0)
}
