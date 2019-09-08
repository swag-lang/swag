#module module368;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	var arr: const [..] s32 = {1,2};	
	@assert(@countof(arr) == 2);
}
