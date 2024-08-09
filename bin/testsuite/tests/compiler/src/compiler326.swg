//#[Swag.PrintBc]
#global testerror "Err0565";
#test
{
	var a: [4] s32;
    #[Swag.Safety("", true)]
	var c = a[5];
}
