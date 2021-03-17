
#global testerror

//#[swag.printbc]
#test
{
	var a: [4, 6] s32;
    #[swag.safety("", true)]
	a[0, 6] = 1;
}
