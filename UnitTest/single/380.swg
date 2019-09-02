#module module380;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	var toto = "1234";
	
	var cpt = 0;
	loop(@op(toto, count))
		cpt += 1;
		
	@assert(cpt == 4);
}
