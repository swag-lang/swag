


struct point
{
	x: s32 = 10
	y: s32 = 200
}

impl point
{
	/*func opInit(self)
	{
		self.x = 10
		self.y = 20
		@print("toto\n")
	}*/
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(pt.x == 10)
	@assert(pt.y == 200)
}