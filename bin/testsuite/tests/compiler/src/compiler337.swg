
;

#global testerror;
//#[Swag.PrintBc]
#test
{
	var a: [4, 6] s32;
    #[Swag.Safety("", true)]
	a[4, 0] = 1;
	//@print(a[0, 0]);
}
