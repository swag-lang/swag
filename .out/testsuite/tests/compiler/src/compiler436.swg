


//#[Swag.PrintBc]
#test
{
	cpt := 0
	for i := 0; i < 10; i += 1
	{
		cpt += 1
		break
		//@assert(false)
	}

	@assert(cpt == 1)
}