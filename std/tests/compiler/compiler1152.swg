#[swag.inline]
func toto(x: bool)
{
	@assert(x)
}

//#[swag.printbc]
#test
{
	toto(true)
}