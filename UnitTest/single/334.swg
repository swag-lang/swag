#module module334;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	var a: [2, 2] s32;	
	//@assert(@op(a, sizeof) == 2'u32 * 2'u32 * @op(s32, sizeof));
	@assert(@op(a, sizeof) == 16);
	var b: [5, 5] bool;	
	@assert(@op(b, sizeof) == 25);
}
