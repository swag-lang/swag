


namespace toto.titi
{
	struct point
	{
		x: s32 = 10
		y: s32
	}
}

//#[Swag.printbc]
#test
{
	var pt: toto.titi.point
	@assert(pt.x == 10)
}