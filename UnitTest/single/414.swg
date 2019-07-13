#module module414

//#[swag.printbytecode]
#unittest error
#test
{
	var d: const [..] s32 = {1, 2, 3, 4};
	@assert(d[2] == 3);
	d[2] = 666;
	//@assert(d[2] == 666);
}