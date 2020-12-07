#module "compiler712"

//#[swag.printbc]
#unittest error
func(T) toto(x: T)->s32
{
	switch x
	{
	case T: 	return 0
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