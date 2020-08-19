#module "compiler724"

func toto(x: s8)
{
}


//#[swag.printbc]
#unittest error
#test
{
	toto(128)
}