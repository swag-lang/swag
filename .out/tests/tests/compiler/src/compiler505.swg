


//#[Swag.printbc]
func T(xx: bool, yy: string, a: ...)
{
	@assert(@countof(a) == 3)
}

//#[Swag.printbc]
#test
{
	T(true, "toto", 10, 100, 600)
}
