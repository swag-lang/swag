


struct(T) Point
{
	x: T
}

impl Point
{
	//#[Swag.waitsem(100)]
	func toto(self)->T
	{
		return self.x
	}
}

//#[Swag.PrintBc]
#test
{
	var pt: Point's32
	pt.x = 5
	@assert(pt.toto() == 5)
}
