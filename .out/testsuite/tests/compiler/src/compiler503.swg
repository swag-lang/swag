


//#[Swag.printbc]
func T(xx: string, xy: bool, a: ...)
{
	@assert(@countof(a) == 3)
	@assert(xx == "true")
	@assert(xy == true)
}

//#[Swag.printbc]
#test
{
	T("true", true, 10, 100, 1000)
}
