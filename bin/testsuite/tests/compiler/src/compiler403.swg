//#[Swag.PrintBc]
#test
{
	let c = 0xFFFFFFFF_FFFFFFFF
    #[Swag.Overflow(true)]
	let a = c << 56
	@assert(a == 0xFF000000_00000000)
}
