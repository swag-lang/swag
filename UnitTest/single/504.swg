#module module504
//#unittest pass semantic

//#[swag.printbc]
func T(a: ...)
{
	@assert(@op(a, count) == 3)
}

//#[swag.printbc]
#test
{
	T(10, 100, 666)
}
