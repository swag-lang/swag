#module module506
//#unittest pass semantic

//#[swag.printbytecode]
func T(xx: bool, a: ...)
{
	@assert(xx == true)
	//@assert(@prop(a, count) == 3)
}

//#[swag.printbytecode]
#test
{
	T(true, 10, 100, 600)
}
