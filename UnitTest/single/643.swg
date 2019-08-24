#module module643
//#unittest pass semantic

struct(T) Point
{
}

impl Point
{
	//#[swag.printbc]
	func toto(self)
	{
	}
}

//#[swag.printbc]
#test
{
	var pt: Point!s32
	pt.toto()
}
