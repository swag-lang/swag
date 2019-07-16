#module module438
#unittest pass semantic

//#[swag.printbytecode]
#test
{
	cpt := 0
	for { i := 0; j := 0; } 
		i < 10
		{ i += 1; j += 2; }
	{
		cpt += 1
	}
	
	@assert(cpt == 5)
}