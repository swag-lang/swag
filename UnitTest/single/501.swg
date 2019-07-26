#module module501
#unittest pass semantic

//#[swag.printbytecode]
func T(a: ...)
{
	var xx: s32;
	@assert(@prop(a, count) == 3)
}

//#[swag.printbytecode]
#test
{
	T(10, 100, 1000)
}
