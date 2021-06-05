


struct point
{
	x: s32 = 60
	y: s32 = 70
}

impl point
{
	func toto(self)
	{
		@assert(self.x == 60)
		@assert(self.y == 70)
	}
}

//#[Swag.printbc]
#test
{
	var pt: [2] point
	pt[0].toto()
}