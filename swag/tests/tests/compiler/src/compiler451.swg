
#global testerror

//#[swag.printbc]
#test
{
	var a: *s32 = null;
    #[swag.safety("", true)]
	a[0] = 0
}