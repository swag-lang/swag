#module module312b;
#unittest backend dll;
//#unittest pass semantic;

var g2: string = "tata";

//#[swag.printbc]
#test
{
	@assert(g2 == "tata");
}
