//#[Swag.PrintBc]
#global testerror "Err0130";
#test
{
	var a: [4] s32;
    #[Swag.Safety("", true)]
	var c = a[5];
}
