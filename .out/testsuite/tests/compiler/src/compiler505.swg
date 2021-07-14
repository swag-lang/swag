


//#[Swag.PrintBc]
func T(xx: bool, yy: string, a: ...)
{
	@assert(@countof(a) == 3)
}

//#[Swag.PrintBc]
#test
{
	T(true, "toto", 10, 100, 600)
}
