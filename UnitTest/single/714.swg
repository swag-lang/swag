#module module714

//#[swag.printbc]
func(T) toto(x: T)->T
{
	return x
}

//#[swag.printbc]
#test
{
	@assert(toto(0) == 0)
	@assert(toto(1) == 1)
	@assert(toto(2) == 2)
}