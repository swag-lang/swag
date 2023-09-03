
//#[Swag.PrintBc]
#global testerror "Err0468";
#test
{
	var a: [4] s32;
    #[Swag.Safety("", true)]
	a[5] = 1;
}
