#[Swag.inline]
func toto()
{
	//a := 0
	return
}

//#[Swag.printbc]
#test
{
	//a := 1
	toto()
	//@assert(a == 1)
}
