


//#[Swag.printbc]
func T(a: ...)
{
	var xx: s32;
	@assert(@countof(a) == 3)
}

//#[Swag.printbc]
#test
{
	T(10, 100, 1000)
}
