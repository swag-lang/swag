

func toto(x: string)
{
	@assert(@countof(x) == 0)
	@assert(@dataof(x) == null)
}

//#[Swag.PrintBc]
#test
{
	toto(null)
}