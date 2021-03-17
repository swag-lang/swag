

//#[swag.printbc]
#global testerror
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

//#[swag.printbc]
#test
{
	@assert(toto(0) == 0)
	@assert(toto(1) == 1)
	@assert(toto(2) == 2)
}