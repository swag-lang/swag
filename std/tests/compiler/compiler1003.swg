using swag

//#[swag.printbc]
func toto(x: ...)
{
	@assert(@kindof(x[0]) == string)
}

//#[swag.printbc]
#test
{
	toto("6")
}
