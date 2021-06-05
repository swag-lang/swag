#[Swag.inline]
func toto(x: bool)
{
	@assert(x)
}

//#[Swag.printbc]
#test
{
	toto(true)
}