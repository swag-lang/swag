#module module334;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var a: [2, 2] s32;	
	//@assert(@prop(a, sizeof) == 2'u32 * 2'u32 * @prop(s32, sizeof));
	@assert(@prop(a, sizeof) == 16);
	var b: [5, 5] bool;	
	@assert(@prop(b, sizeof) == 25);
}
