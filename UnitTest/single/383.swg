#module module383;
//#unittest pass semantic;

#[swag.printbytecode]
#test
{
	var cpt: u32 = 0;
	loop(5)
	{
		cpt += index;
	}
	
	@assert(cpt == 1 + 2 + 3 + 4);
}
