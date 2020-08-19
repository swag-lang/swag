#module "compiler1137"
#unittest error

var b: s32

#[swag.inline]
func toto()
{
	b := 666
}

//#[swag.printbc]
#test
{
	toto()
}
