#module module504
//#unittest pass semantic

//#[swag.printbytecode]
func T(a: ...)
{
	@assert(@prop(a, count) == 3)
}

//#[swag.printbytecode]
#test
{
	T(10, 100, 666)
}
