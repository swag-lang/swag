


struct toto
{
	x: s32
	y: s32
}

impl toto
{
	func fff(self)
	{
		@assert(self.x == 100)
		@assert(self.y == 200)
	}
}

//#[Swag.PrintBc]
#test
{
	using toto
	var pt: toto
	pt.x = 100
	pt.y = 200
	fff(pt)
}