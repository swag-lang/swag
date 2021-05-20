//#[Swag.printbc]
#global testerror;
#test
{
	var a: [4] s32;
    #[Swag.safety("", true)]
	var c = a[5];
}
