#global testerror "Err0473"

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