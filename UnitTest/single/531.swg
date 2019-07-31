#module module531
#unittest pass semantic

struct point
{
	var x: s32
}

impl point
{
	//#[swag.printbytecode]
	func set(self, x: s32)
	{
		self.x = x
	}
}

//#[swag.printbytecode]
#test
{
	var pt: point
	point.set(pt, 10)
	@assert(pt.x == 10)
}