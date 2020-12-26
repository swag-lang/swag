;
;

//#[swag.printbc]
#test
{
	var cpt: uint = 0;
	loop(5)
	{
		cpt += @index;
	}

	@assert(cpt == 1 + 2 + 3 + 4);
}
