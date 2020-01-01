using swag

#[swag.printbc]
func toto(x: ...)
{
	@assert(x[0] is string)
}

//#[swag.printbc]
#test
{
	toto("6")
}
