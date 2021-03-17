;
;

//#[swag.printbc]
#test
{
	var toto = "1234";
	
	var cpt = 0;
	loop(@countof(toto))
		cpt += 1;
		
	@assert(cpt == 4);
}
