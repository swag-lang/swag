#module module358;
//#unittest pass semantic;

var a = [1, 2];

//#[swag.printbytecode]
#test
{
	var ptr1 = &a[1];
	@assert(ptr1[0] == 2);
	var ptr0 = &a[0];
	@assert(ptr0[0] == 1);
}
