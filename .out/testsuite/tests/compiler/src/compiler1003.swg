using Swag

//#[Swag.printbc]
func toto(x: ...)
{
	@assert(@kindof(x[0]) == string)
}

//#[Swag.printbc]
#test
{
	toto("6")
}
