
;

//#[Swag.PrintBc]
#global testerror "Err0160";
#test
{
	var cpt = 0;

	for(-10)
	{
		cpt += 1;
	}

	@assert(cpt == 10);
}
