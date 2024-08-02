#global testerror "Err0181"

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