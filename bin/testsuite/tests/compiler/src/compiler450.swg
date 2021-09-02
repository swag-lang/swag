
#global testerror

//#[Swag.PrintBc]
#test
{
	var a: *s32 = null;
    #[Swag.Safety("", true)]
	if(a[0] == 0)
	{
	}
}