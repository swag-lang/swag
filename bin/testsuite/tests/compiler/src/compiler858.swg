

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

//#[Swag.PrintBc]
#test
{
	G = 0

	for(10)
	{
		var pt: Point{10}
	}

	@assert(G == 10)
}