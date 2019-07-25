#module module354;
//#unittest pass semantic;

var a = ["0", "1.0"];

//#[swag.printbytecode]
#test
{
	@assert(a[0] == "0");
	@assert(a[1] == "1.0");
}
