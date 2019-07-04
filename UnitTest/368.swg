#module module368;
//#unittest pass semantic;

#[swag.printbytecode]
#test
{
	var arr: [..] s32 = {1,2};	
	var t = {1,2,3,4};	
	@assert(@prop(arr, count) == 2);
    arr = t;
}
