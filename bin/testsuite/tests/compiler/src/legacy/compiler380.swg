;
;

//#[Swag.PrintBc]
#test
{
	let toto = "1234";

	var cpt = 0;
	for(@countof(toto)):
		cpt += 1;

	@assert(cpt == 4);
}
