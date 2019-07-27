#module module505
//#unittest pass semantic

//#[swag.printbytecode]
func T(xx: bool, yy: string, a: ...)
{
	@assert(@prop(a, count) == 3)
}

//#[swag.printbytecode]
#test
{
	T(true, "toto", 10, 100, 600)
}
