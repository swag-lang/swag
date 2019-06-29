#module module333;
//#unittest pass semantic;

var a: [2] s32 = {50, 60};

//#[swag.printbytecode]
#test
{
	@assert(a[0] == 50);
	@assert(a[1] == 60);
}
