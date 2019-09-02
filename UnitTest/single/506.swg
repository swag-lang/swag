#module module506
//#unittest pass semantic

//#[swag.printbc]
func T(xx: bool, a: ...)
{
	@assert(xx == true)
	@assert(@op(a, count) == 3)
}

//#[swag.printbc]
#test
{
	T(true, 10, 100, 600)
}
