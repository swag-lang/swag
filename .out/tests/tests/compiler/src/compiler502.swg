


//#[Swag.printbc]
#global testerror
func T(xx: string, a: ...)
{
	@assert(@countof(a) == 3)
	@print(cast(s64) @countof(a))
}

//#[Swag.printbc]
#test
{
	T(true, 10, 100, 1000)
}
