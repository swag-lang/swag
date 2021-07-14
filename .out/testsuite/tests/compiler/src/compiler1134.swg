#[Swag.inline]
func toto()
{
	//a := 0
	return
}

//#[Swag.PrintBc]
#test
{
	//a := 1
	toto()
	//@assert(a == 1)
}
