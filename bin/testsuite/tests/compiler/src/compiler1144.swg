#global testerror "Err0170"

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