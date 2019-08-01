#module module541
//#unittest pass semantic

namespace toto.titi
{
	struct point
	{
		var x: s32 = 10
		var y: s32
	}
}

//#[swag.printbytecode]
#test
{
	var pt: toto.titi.point
	@assert(pt.x == 10)
}