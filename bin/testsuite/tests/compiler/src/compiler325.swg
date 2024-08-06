
//#[Swag.PrintBc]
#global testerror "Err0562";
#test
{
	var a: [4] s32;
    #[Swag.Safety("", true)]
	a[5] = 1;
}
