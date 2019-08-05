#module module560
//#unittest pass semantic

struct point
{
	var x: s32 = 60
	var y: s32 = 70
}

impl point
{
	func toto(self)
	{
		@assert(self.x == 60)
		@assert(self.y == 70)
	}
}

//#[swag.printbytecode]
#test
{
	var pt: [2] point
	pt[0].toto()
}