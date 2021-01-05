
;

//#[swag.printbc]
#global testerror;
#test
{
	var a: [4] s32;
    #[swag.safety(true)]
	var c = a[5];
}
