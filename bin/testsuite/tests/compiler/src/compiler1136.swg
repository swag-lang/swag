#[Swag.Inline]
func toto()
{
	var b = 666
	return
}

//#[Swag.PrintBc]
#test
{
	var a = 1
	toto()
	@assert(a == 1)
}
