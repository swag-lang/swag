;
;

//#[Swag.printbc]
#test
{
	var a: [2, 2] s32;
	//@assert(@sizeof(a) == 2'u32 * 2'u32 * @sizeof(s32));
	@assert(@sizeof(a) == 16);
	var b: [5, 5] bool;
	@assert(@sizeof(b) == 25);
}
