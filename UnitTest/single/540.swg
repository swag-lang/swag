#module module540
//#unittest pass semantic

namespace toto
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
	var pt: toto.point
	@assert(pt.x == 10)
}