#module module379;
//#unittest pass semantic;

//#[swag.printbytecode]
#unittest error;
#test
{
	var cpt = 0;
	
	loop(-10)
	{
		cpt += 1;
	}
	
	@assert(cpt == 10);
}
