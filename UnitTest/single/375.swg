#module module375;
//#unittest pass semantic;

//#[swag.printbytecode]
func a (ptr: const [..] s32, cpt: s32)
{
	@assert(@prop(ptr, count) == cast(u32) (cpt));
}

//#[swag.printbytecode]
#test
{
	var slice: const [..] s32;
	slice = {1,2,3};
	a(slice, 3);
	slice = {1,2,3,4,5,6};
	a(slice, 6);
}
