struct point
{
	x: s32
}

impl point
{
	//#[Swag.printbc]
	func set(self, _x: s32)
	{
		@assert(self.x == 100)
		self.x = _x
	}
}

//#[Swag.printbc]
#test
{
	var pt: point
	pt.x = 100
	point.set(pt, 10)
	@assert(pt.x == 10)
}