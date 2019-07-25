#module module367;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var arr: [] s32 = [1,2];
	@assert(@prop(arr, count) == 2);
	@assert(@prop(arr, sizeof) == 8);
}
