#module module369;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var slice: [..] s32;	
	var t = {1,2,3,4};	
    slice = t;	
	@assert(@prop(slice, count) == 4);
	
	var t1 = {1,2,3,4,1,2,3,4};	
	slice = t1;	
	@assert(@prop(slice, count) == 8);
}
