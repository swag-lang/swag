#[Swag.Inline]
func toto(x: s64, y: s64)
{
	@assert((x * 2) + y == 5)
}

//#[Swag.PrintBc]
#test
{
	toto(x: 2, y: 1)
	toto(y: 1, x: 2)
}