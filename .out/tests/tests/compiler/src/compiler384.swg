;

//#[Swag.printbc]
#test
{
	var cpt: uint = 0;
	while(cpt < 10)
	{
		cpt += @index;
	}

	@assert(cpt == 1 + 2 + 3 + 4);
}
