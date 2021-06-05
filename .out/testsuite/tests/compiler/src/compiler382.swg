;
;

//#[Swag.printbc]
#test
{
	var cpt = 0;
	var b = 10'u32;
	loop(b)
	{
		cpt += 1;
	}

	@assert(cpt == 10);
	@assert(b == 10);
}
