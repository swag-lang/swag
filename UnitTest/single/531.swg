#module module531
#unittest pass semantic

struct point
{
	var x: s32
}

impl point
{
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
	@print(pt.x)
}