func F(a: nl string = null)
{
	@assert(@countof(a) == 0)
	@assert(@dataof(a) == null)
}

//#[Swag.PrintBc]
#test
{
	F()
}