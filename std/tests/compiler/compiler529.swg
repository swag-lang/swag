


struct toto
{
	x: s32
	y: s32
}

impl toto
{
	func fff(self)
	{
		//@print(self.x)
	}
}

#test
{
	using toto
	var pt: toto
	pt.x = 100
	pt.y = 200
	fff(pt)
}