#module module541
//#unittest pass semantic

namespace toto.titi
{
	struct point
	{
		x: s32 = 10
		y: s32
	}
}

//#[swag.printbc]
#test
{
	var pt: toto.titi.point
	@assert(pt.x == 10)
}