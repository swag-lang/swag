#module module370;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var slice: const [..] s32;	
    slice = {1, 2, 3};	
	@assert(@prop(slice, count) == 3);
	slice = {1, 2, 3, 4};
	@assert(@prop(slice, count) == 4);
}
