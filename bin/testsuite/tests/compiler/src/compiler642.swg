struct(T) Point
{
	x: T = 65
}

impl Point
{
	//#[Swag.PrintBc]
	func toto(self)->T
	{
		return self.x
	}
}

//#[Swag.PrintBc]
#test
{
	var pt: Point's32
	@assert(Point.toto(pt) == 65)
	var pt1: Point'f32
	@assert(Point.toto(pt1) == 65.0)
}
