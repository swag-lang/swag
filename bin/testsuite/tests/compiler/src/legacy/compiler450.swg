#global skip
#global testerror

//#[Swag.PrintBc]
#test
{
	var a: *s32 = null;
    #[Swag.Safety(.All, .All, true)]
	if(a[0] == 0)
	{
	}
}