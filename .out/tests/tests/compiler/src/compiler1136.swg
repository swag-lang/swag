#[swag.inline]
func toto()
{
	b := 666
	return
}

//#[swag.printbc]
#test
{
	a := 1
	toto()
	@assert(a == 1)
}
