

//#[swag.printbc]
#global testerror
func(T) toto(x: T)->s32
{
	return T
}

//#[swag.printbc]
#test
{
	@assert(toto(0) == 0)
	@assert(toto(1) == 1)
	@assert(toto(2) == 2)
}