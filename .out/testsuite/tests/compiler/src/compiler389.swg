;
;

//#[Swag.PrintBc]
#test
{
	var cpt: const [..] s32 = @[1, 2, 4];
	var ptr = @dataof(cpt);
	@assert(ptr[0] == 1);
	@assert(ptr[1] == 2);
	@assert(ptr[2] == 4);
	@assert(cpt[0] == 1);
	@assert(cpt[1] == 2);
	@assert(cpt[2] == 4);
}
