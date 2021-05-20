


struct(T) Point
{
	x: T
}

impl Point
{
	//#[Swag.printbc]
	func toto(self)->T
	{
		return self.x
	}
}

//#[Swag.printbc]
#test
{
	var pt: Point's32
	pt.x = 5
	@assert(pt.toto() == 5)
}
