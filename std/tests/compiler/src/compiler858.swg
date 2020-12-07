

var G: s32

struct Point
{
	x: s32
}

impl Point
{
	func opDrop(self)
	{
		@assert(self.x == 10)
		G += 1
	}
}

//#[swag.printbc]
#test
{
	G = 0

	loop(10)
	{
		var pt: Point{10}
	}

	@assert(G == 10)
}