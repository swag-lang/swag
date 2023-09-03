
#global testerror "Err0468"

//#[Swag.PrintBc]
#test
{
	var a: [4, 6] s32;
    #[Swag.Safety("", true)]
	a[0, 6] = 1;
}
