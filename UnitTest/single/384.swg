#module module384;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var cpt: u32 = 0;
	while(cpt < 10)
	{
		cpt += index;
	}
	
	@assert(cpt == 1 + 2 + 3 + 4);
}
