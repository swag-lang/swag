#module module530
//#unittest pass semantic

struct toto
{
	var x: s32
	var y: s32
}

impl toto
{
	func fff(self)
	{
		@assert(self.x == 100)
		@assert(self.y == 200)
	}
}

//#[swag.printbytecode]
#test
{
	using toto
	var pt: toto
	pt.x = 100
	pt.y = 200
	fff(pt)
}