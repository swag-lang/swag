

var G: s32

struct Point
{
	x: s32
}

impl Point
{
	func opDrop(self)
	{
		@assert(self.x == 1)
		G += 1
	}
}

//#[Swag.printbc]
#test
{
	G = 0

	{
		var pt: Point{1}
		var pt1: Point{1}
	}

	@assert(G == 2)
}