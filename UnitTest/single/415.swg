#module module415

//#[swag.printbytecode]
#test
{
	const cc = {1, 2, 3, 4};
	var d: [..] s32 = cc;
	@assert(d[2] == 3);
	d[2] = 666;
	@assert(d[2] == 666);
}