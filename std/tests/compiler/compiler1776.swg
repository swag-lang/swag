//#[swag.printbc]
#test
{
	var c = 0xFFFFFFFF_FFFFFFFF;
	var a = c << 56;
	@assert(a == 0xFF000000_00000000);
}
