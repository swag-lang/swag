
#global testerror

//#[Swag.printbc]
#test
{
	var a: *s32 = null;
    #[Swag.safety("", true)]
	if(a[0] == 0)
	{
	}
}