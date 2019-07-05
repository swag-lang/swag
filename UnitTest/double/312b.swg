#module module312b;
#unittest backend dll;
//#unittest pass semantic;

var g2: string = "tata";

//#[swag.printbytecode]
#test
{
	@assert(g2 == "tata");
}
