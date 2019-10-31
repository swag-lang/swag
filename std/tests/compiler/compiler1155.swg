#[swag.inline]
func toto(x: s64, y: s64 = 666)
{
	@assert(x + y == 667)
}

//#[swag.printbc]
#test
{
	toto(1)
}