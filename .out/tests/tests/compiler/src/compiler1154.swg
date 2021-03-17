#[swag.inline]
func toto(x: s64 = 666)
{
	@assert(x == 666)
}

//#[swag.printbc]
#test
{
	toto()
}