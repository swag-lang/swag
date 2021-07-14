//#[Swag.PrintBc]
func(T) toto(x: T)->s32
{
	switch x
	{
	case 0: 	return 0
	case 1: 	return 1
	default: 	return 2
	}

	return 0
}

//#[Swag.PrintBc]
#test
{
	@assert(toto(0'u32) == 0)
	@assert(toto(1'u32) == 1)
	@assert(toto(2'u32) == 2)
}