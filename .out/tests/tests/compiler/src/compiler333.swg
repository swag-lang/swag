;
;

var a: [2] s32 = @[50, 60];

//#[swag.printbc]
#test
{
	@assert(a[0] == 50);
	@assert(a[1] == 60);
}
