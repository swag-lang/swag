#global testerror "Err0131"

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