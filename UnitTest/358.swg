#module module358;
//#unittest pass semantic;

var a = {1, 2};

//#[swag.printbytecode]
#test
{
	var ptr = &a[1];
	@assert(ptr[0] == 2);
}
