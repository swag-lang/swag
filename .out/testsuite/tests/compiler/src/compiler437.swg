


//#[Swag.PrintBc]
#test
{
	cpt := 0
	for i := 0; i < 10; i += 1
	{
		if i >= 5 continue
		cpt += 1
	}

	@assert(cpt == 5)
}