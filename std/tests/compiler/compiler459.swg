


//#[swag.printbc]
#test
{
	var c = @[1, 2, 3];
	var d: [..] s32 = @mkslice(&c[0], 3)
	@assert(@countof(d) == 3)
	@assert(c[0] == 1)
	@assert(c[1] == 2)
	@assert(c[2] == 3)
}