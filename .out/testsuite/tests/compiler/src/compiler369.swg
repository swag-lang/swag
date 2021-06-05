;
;

//#[Swag.printbc]
#test
{
	var slice: [..] s32;
	var t = @[1,2,3,4];
    slice = cast([..]s32) t;
	@assert(@countof(slice) == 4);

	var t1 = @[1,2,3,4,1,2,3,4];
	slice = cast([..]s32) t1;
	@assert(@countof(slice) == 8);
}
