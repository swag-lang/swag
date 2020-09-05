struct point
{
	x: s32
	y: s32
}

impl point
{
	//#[swag.printbc]
	func set(self, _x: s32, _y: s32)
	{
		self.x = _x
		self.y = _y
	}
}

//#[swag.printbc]
#test
{
	var pt: point
	point.set(pt, 10, 20)
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}