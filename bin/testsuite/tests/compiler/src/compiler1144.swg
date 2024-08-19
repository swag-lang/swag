#global testerror "Err0141"

#[Swag.Inline]
func toto()
{
	//@print(x)
}

//#[Swag.PrintBc]
#test
{
	var x = &toto
}