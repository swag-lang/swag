

//#[swag.printbc]
#global testerror
#test
{
	var a: [..] s32 = null;
    #[swag.safety("", true)]
	if(a[0] == 0)
	{
	}
}