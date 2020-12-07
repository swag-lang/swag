

struct Point
{
	x: s32
}

impl Point
{
	func opDrop(using self)
	{
		x = 100
	}
}

//#[swag.printbc]
#test
{
	var pt : Point
	ptr := &pt
	@drop(ptr)
	@assert(pt.x == 100)
}