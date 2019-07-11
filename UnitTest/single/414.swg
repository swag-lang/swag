#module module414

//#[swag.printbytecode]
#test
{
	var d: [..] s32 = {1, 2, 3, 4};
	@assert(d[2] == 3);
	d[2] = 666;
	@assert(d[2] == 666);
}