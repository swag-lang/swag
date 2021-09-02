//#[Swag.PrintBc]
#test
{
	var c = 0xFFFFFFFF_FFFFFFFF;
    #[Swag.Safety("overflow", false)]
	var a = c >> 56;
	@assert(a == 0xFF);
}
