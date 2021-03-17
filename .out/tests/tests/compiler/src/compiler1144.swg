
#global testerror

#[swag.inline]
func toto()
{
	//@print(x)
}

//#[swag.printbc]
#test
{
	x := &toto
}