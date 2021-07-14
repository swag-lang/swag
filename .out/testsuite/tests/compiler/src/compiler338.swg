
#global testerror

//#[Swag.PrintBc]
#test
{
	var a: [4, 6] s32;
    #[Swag.safety("", true)]
	a[0, 6] = 1;
}
