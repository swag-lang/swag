#module "compiler837"

//#[swag.printbc]
#unittest error
#test
{
	loop(10)
	{
		break
		a := 0
	}
}