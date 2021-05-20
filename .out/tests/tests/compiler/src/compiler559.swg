


struct point
{
	x: s32 = 60
	y: s32
}

impl point
{
	func toto(self)
	{
		@assert(self.x == 60)
	}
}

//#[Swag.printbc]
#test
{
	var pt: point
	pt.toto()
}