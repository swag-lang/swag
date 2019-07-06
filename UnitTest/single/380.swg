#module module380;
//#unittest pass semantic;

//#[swag.printbytecode]
#unittest error;
#test
{
	var toto = "1234";
	
	var cpt = 0;
	loop(@prop(toto, count))
		cpt += 1;
		
	@assert(cpt == 4);
}
