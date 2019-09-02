#module module367;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	var arr: [] s32 = [1,2];
	@assert(@op(arr, count) == 2);
	@assert(@op(arr, sizeof) == 8);
}
