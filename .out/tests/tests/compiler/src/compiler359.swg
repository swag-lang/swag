;
;

var a = @[1, 2];

//#[swag.printbc]
#test
{
	var ptr1 = &a;
	@assert(ptr1[0] == 1);
	@assert(ptr1[1] == 2);
}
