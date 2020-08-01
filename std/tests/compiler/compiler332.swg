;
;

//#[swag.printbc]
#test
{
	var a: [2] s32 = @[50, 60];
	var b = 666;
	@assert(a[0] == 50);
	@assert(a[1] == 60);
	@assert(b == 666);
}
