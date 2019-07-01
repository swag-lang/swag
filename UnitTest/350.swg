#module module350;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	const a = 666;
	var b = #run a;	
	@assert(b == 666);
}
