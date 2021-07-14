


//#[Swag.PrintBc]
func T(xx: bool, a: ...)
{
	@assert(xx == true)
	@assert(@countof(a) == 3)
}

//#[Swag.PrintBc]
#test
{
	T(true, 10, 100, 600)
}
