#module module641
//#unittest pass semantic

struct(T) Point
{
	var x: T = 1
}

impl Point
{
	//#[swag.printbc]
	func toto(x: T)->T
	{
		return x
	}
}

//#[swag.printbc]
#test
{
	var pt: Point!s32
	@assert(Point.toto(10) == 10)
	@assert(Point.toto(10.0) == 10.0)
}
