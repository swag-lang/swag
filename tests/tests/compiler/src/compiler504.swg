


//#[swag.printbc]
func T(a: ...)
{
	@assert(@countof(a) == 3)
}

//#[swag.printbc]
#test
{
	T(10, 100, 666)
}
