#module module525
#unittest pass semantic

struct point
{
	var x: s32 = 10
	var y: s32 = 100
}

impl point
{
}

#[swag.printbytecode]
#test
{
	var pt: point
	//@assert(pt.x == 10)
}
