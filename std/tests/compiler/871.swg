

func toto(x: string)
{
	@assert(@countof(x) == 0)
	@assert(@dataof(x) == null)
}

//#[swag.printbc]
#test
{
	toto(null)
}