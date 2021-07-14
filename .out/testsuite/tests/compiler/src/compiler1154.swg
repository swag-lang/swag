#[Swag.inline]
func toto(x: s64 = 666)
{
	@assert(x == 666)
}

//#[Swag.PrintBc]
#test
{
	toto()
}