

func toto(x: string)
{
	@assert(@countof(x) == 0)
	@assert(@dataof(x) == null)
}

//#[Swag.printbc]
#test
{
	toto(null)
}