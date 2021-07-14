

//#[Swag.PrintBc]
func(T) toto(x: T)->T
{
	return x
}

//#[Swag.PrintBc]
#test
{
	@assert(toto(0'u8) == 0)
	@assert(toto("1") == "1")
	@assert(toto(true) == true)
}