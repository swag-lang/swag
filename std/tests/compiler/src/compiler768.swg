var test: s32

struct Point
{
	x: s32 = 1
	y: s32
}

struct Point1
{
	x: s32 = 1
	y: s32
	pt: Point
	pt1: Point
}

//#[swag.printbc]
#test
{	
	var pt1: Point1
	var pt2: Point1
	
	test = 0
	pt2 = pt1
	@assert(test == 2)
	@assert(pt2.pt.x == 2)
	@assert(pt2.pt.y == 4)
	@assert(pt2.pt1.x == 2)
	@assert(pt2.pt1.y == 4)	
}

impl Point
{
	//#[swag.waitsem(100)]
	func opPostCopy(self)
	{
		//@print("postcopy!\n")
		test += 1
		self.x = 2
		self.y = 4
	}
}