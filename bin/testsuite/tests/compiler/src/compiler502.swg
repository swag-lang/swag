


//#[Swag.PrintBc]
#global testerror "Err0742"
func T(xx: string, a: ...)
{
	@assert(@countof(a) == 3)
	@print(cast(s64) @countof(a))
}

//#[Swag.PrintBc]
#test
{
	T(true, 10, 100, 1000)
}
