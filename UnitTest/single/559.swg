#module module559
//#unittest pass semantic

struct point
{
	var x: s32 = 60
	var y: s32
}

impl point
{
	func toto(self)
	{
		@assert(self.x == 60)
	}
}

//#[swag.printbytecode]
#test
{
	var pt: point
	pt.toto()
}