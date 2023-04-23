//#[Swag.PrintBc]
#test
{
	var c = 0xFFFFFFFF_FFFFFFFF;
    #[Swag.Overflow(true)]
	var a = c >> 56;
	@assert(a == 0xFF);
}
