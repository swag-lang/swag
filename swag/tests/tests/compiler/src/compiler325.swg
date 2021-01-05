
;

//#[swag.printbc]
#global testerror;
#test
{
	var a: [4] s32;
    #[swag.safety(true)]
	a[5] = 1;
}
