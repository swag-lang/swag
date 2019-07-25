#module module351;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var a = [0, 1];
	@assert(a[0] == 0);
	@assert(a[1] == 1);
}
