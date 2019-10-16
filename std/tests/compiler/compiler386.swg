;
;

//#[swag.printbc]
#test
{
	var cpt = 0;
	loop("toto")
	{
		cpt += 1;
	}
	
	@assert(cpt == 4);
}
