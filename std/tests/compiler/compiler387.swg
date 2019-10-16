;
;

//#[swag.printbc]
#test
{
	var cpt = 0;
	var l = "titi";
	loop(l)
	{
		cpt += 1;
	}
	
	@assert(cpt == 4);
}
