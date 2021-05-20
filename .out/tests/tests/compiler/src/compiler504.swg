


//#[Swag.printbc]
func T(a: ...)
{
	@assert(@countof(a) == 3)
}

//#[Swag.printbc]
#test
{
	T(10, 100, 666)
}
