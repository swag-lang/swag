//#[Swag.PrintBc]
#global testerror "Err0489";
#test
{
	var a: [4] s32;
    #[Swag.Safety("", true)]
	var c = a[5];
}
