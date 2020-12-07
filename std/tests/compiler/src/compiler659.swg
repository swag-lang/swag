#module "compiler659"


enum OT
{
	A = 1
	B
}

//#[swag.printbc]
#unittest error
#test
{
	var pt: [1] OT
}
