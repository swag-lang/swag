


//#[swag.printbc]
func T(xx: bool, a: ...)
{
	@assert(xx == true)
	@assert(@countof(a) == 3)
}

//#[swag.printbc]
#test
{
	T(true, 10, 100, 600)
}
