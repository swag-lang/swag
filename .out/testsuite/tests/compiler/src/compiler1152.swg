#[Swag.inline]
func toto(x: bool)
{
	@assert(x)
}

//#[Swag.PrintBc]
#test
{
	toto(true)
}