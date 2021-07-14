


//#[Swag.PrintBc]
#test
{
	cpt := 0
	switch(5)
	{
	case 5:
		cpt = 1;
		break;
		//@assert(false);
		//cpt = 2;
	case 6:
		cpt = 2;
	}

	@assert(cpt == 1)
}