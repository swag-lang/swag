#module module369;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var slice: [..] s32;	
	var t = {1,2,3,4};	
    slice = cast([..]s32) t;	
	@assert(@prop(slice, count) == 4);
	
	var t1 = {1,2,3,4,1,2,3,4};	
	slice = cast([..]s32) t1;	
	@assert(@prop(slice, count) == 8);
}
