#[swag.inline]
func toto()
{
	//b := 0
	return
}

//#[swag.printbc]
#test
{
	a := 1
	toto()
	@assert(a == 1)
}
