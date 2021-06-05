#[Swag.inline]
func toto(x: s64, y: s64 = 666)
{
	@assert(x + y == 667)
}

//#[Swag.printbc]
#test
{
	toto(1)
}