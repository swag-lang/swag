;
;

//#[swag.printbc]
#test
{
	var slice: const [..] s32;
    slice = @[1, 2, 3];
	@assert(@countof(slice) == 3);
	slice = @[1, 2, 3, 4];
	@assert(@countof(slice) == 4);
}
