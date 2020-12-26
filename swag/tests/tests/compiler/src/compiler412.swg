
#unittest error

//#[swag.printbc]
#test
{
	var c = @[666, 666];
	var d: [..] s32 = c;

    #[swag.safety(true)]
    d[2] = 100;

	//var c = {0};
	//var d: [..] *s32 = {&c[0]};
	//d[0,0] = 1;

	//var c: [..] s32 = {1};
	//@assert(c[0] == 1);
	//c[0] = 10;
	//@assert(c[0] == 10);
}