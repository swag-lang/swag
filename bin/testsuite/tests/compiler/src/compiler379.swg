
;

//#[Swag.PrintBc]
#global testerror "Err0145";
#test
{
	var cpt = 0;

	loop(-10)
	{
		cpt += 1;
	}

	@assert(cpt == 10);
}
