//#[Swag.PrintBc]
#test
{
	let c = 666;
	let d: [..] s32 = [1, c, 2];

	@assert(d[0] == 1);
	@assert(d[2] == 2);
	@assert(d[1] == 666);
}
