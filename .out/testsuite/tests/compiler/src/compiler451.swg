
#global testerror

//#[Swag.PrintBc]
#test
{
	var a: *s32 = null;
    #[Swag.safety("", true)]
	a[0] = 0
}