
//#[Swag.printbc]
#global testerror;
#test
{
	var a: [4] s32;
    #[Swag.safety("", true)]
	a[5] = 1;
}
