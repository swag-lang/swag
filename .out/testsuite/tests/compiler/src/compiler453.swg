func F(a: string = null)
{
	@assert(@countof(a) == 0)
	@assert(@dataof(a) == null)
}

//#[Swag.printbc]
#test
{
	F()
}