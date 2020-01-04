

//#[swag.printbc]
func(T) toto(x: T)->T
{
	return x
}

//#[swag.printbc]
#test
{
	@assert(toto(0'u8) == 0)
	@assert(toto("1") == "1")
	@assert(toto(true) == true)
}