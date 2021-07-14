#[Swag.Inline]
func toto()
{
	b := 666
	return
}

//#[Swag.PrintBc]
#test
{
	a := 1
	toto()
	@assert(a == 1)
}
