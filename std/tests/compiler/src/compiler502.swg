


//#[swag.printbc]
#unittest error
func T(xx: string, a: ...)
{
	@assert(@countof(a) == 3)
	@print(cast(s64) @countof(a))
}

//#[swag.printbc]
#test
{
	T(true, 10, 100, 1000)
}
