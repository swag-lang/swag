

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
	var pt : [3] Point
	ptr := &pt
	@drop(ptr, 3)
	@assert(pt[0].x == 100)
	@assert(pt[1].x == 100)
	@assert(pt[2].x == 100)
}