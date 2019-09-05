#module module533
//#unittest pass semantic

struct point
{
	x: s32
	y: s32
}

impl point
{
	//#[swag.printbc]
	func set(self, x: s32, y: s32)
	{
		self.x = x
		self.y = y
	}
}

//#[swag.printbc]
#test
{
	var pt: point
	pt.set(10, 20)
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}