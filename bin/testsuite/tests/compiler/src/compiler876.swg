

func toto(x: s32)->s32 => x * x

//#[Swag.PrintBc]
#test
{
	@assert(toto(4) == 16)
	@assert(toto(2) == 4)
}