
//#[Swag.PrintBc]
#global testerror;
#test
{
	var a: [4] s32;
    #[Swag.Safety("", true)]
	a[5] = 1;
}
