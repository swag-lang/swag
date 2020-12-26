;
;

//#[swag.printbc]
#test
{
	var a: [4] s32;
	a[0] = 1;
	a[1] = 2;
	a[2] = 20;
	a[3] = 30;
	@assert(a[0] == 1);
	@assert(a[1] == 2);
	@assert(a[2] == 20);
	@assert(a[3] == 30);
}
