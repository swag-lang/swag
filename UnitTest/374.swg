#module module374;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	const a = 666;
	var slice = {1, a, 2};
	@assert(slice[1] == 666);
}
