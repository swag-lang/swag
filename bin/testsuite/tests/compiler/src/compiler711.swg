#global testerror "Err0227"

func(T) toto(x: T)->s32
{
	switch T
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
	@assert(toto(0's32) == 0)
	@assert(toto(1's32) == 1)
	@assert(toto(2's32) == 2)
}