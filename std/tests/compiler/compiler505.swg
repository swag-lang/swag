


//#[swag.printbc]
func T(xx: bool, yy: string, a: ...)
{
	@assert(@countof(a) == 3)
}

//#[swag.printbc]
#test
{
	T(true, "toto", 10, 100, 600)
}
