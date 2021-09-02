
;

//#[Swag.PrintBc]
#global testerror;
#test
{
	var cpt = 0;

	loop(10.0)
	{
		cpt += 1;
	}

	@assert(cpt == 10);
}
