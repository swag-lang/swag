;
;

//#[swag.printbc]
#test
{
	var toto = "1234";
	
	var cpt = 0;
	loop(@countof(toto))
	{
		//@print("toto");
		cpt += 1;
		if(cpt == 2)
			break;
	}
	
	//@print(cpt);
	@assert(cpt == 2);
}
