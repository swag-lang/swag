#module module642
//#unittest pass semantic

struct(T) Point
{
	var x: T = 65
}

impl Point
{
	#[swag.printbc]
	func toto(self)->T
	{
		return self.x
	}
}

//#[swag.printbc]
#test
{
	var pt: Point!s32
	@assert(Point.toto(pt) == 65)
}
