
;

#global testerror;
//#[Swag.printbc]
#test
{
	var a: [4, 6] s32;
    #[Swag.safety("", true)]
	a[4, 0] = 1;
	//@print(a[0, 0]);
}
