

var test: s32

struct Point
{
	x: s32 = 1
	y: s32
}

impl Point
{
	func opPostCopy(self)
	{
		//@print("postcopy!\n")
		test = 1
	}
}

//#[Swag.PrintBc]
#test
{
	var pt1: Point
	var pt2: Point

	test = 0
	pt2 = pt1
	@assert(test == 1)
}
