#module "compiler656"


enum OT
{
	A = 1
	B
}

//#[swag.printbc]
#unittest error
#test
{
	var a: OT
}
