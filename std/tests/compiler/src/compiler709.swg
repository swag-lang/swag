#module "compiler709"

//#[swag.printbc]
#unittest error
#test
{
	loop(i: 2)
	{
		loop(i: 2)
		{	
		}	
	}	
}