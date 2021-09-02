using Swag

//#[Swag.PrintBc]
func toto(x: ...)
{
	@assert(@kindof(x[0]) == string)
}

//#[Swag.PrintBc]
#test
{
	toto("6")
}
