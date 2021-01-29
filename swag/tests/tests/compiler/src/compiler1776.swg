//#[swag.printbc]
#test
{
	var c = 0xFFFFFFFF_FFFFFFFF;
    #[swag.safety("of", false)]
	var a = c << 56;
	@assert(a == 0xFF000000_00000000);
}
