#module module524
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	var pt: [10] s32
	@assert(pt[0] == 0)
	@assert(pt[9] == 0)
}
