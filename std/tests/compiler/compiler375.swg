;
;

//#[swag.printbc]
func a (ptr: const [..] s32, cpt: s32)
{
	@assert(@countof(ptr) == cast(u32) (cpt));
}

//#[swag.printbc]
#test
{
	var slice: const [..] s32;
	slice = @[1,2,3];
	a(slice, 3);
	slice = @[1,2,3,4,5,6];
	a(slice, 6);
}
