#global testerror "Err0093"

//#[Swag.PrintBc]
#test
{
	var d: const [..] s32 = [1, 2, 3, 4]
	@assert(d[2] == 3)
	d[2] = 666;
}