#module "compiler1144"
#unittest error

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