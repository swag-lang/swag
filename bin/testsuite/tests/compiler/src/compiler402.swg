//#[Swag.PrintBc]
#test
{
	let c = 0xFFFFFFFF_FFFFFFFF;
    #[Swag.AllowOverflow(true)]
	let a = c >> 56;
	@assert(a == 0xFF);
}
