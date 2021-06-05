


//#[Swag.printbc]
#test
{
	cpt := 0
	for { i := 0; j := 0; }
		i < 10
		{ i += 1; j += 2; }
	{
		cpt += j
	}

	@assert(cpt == 90)
}