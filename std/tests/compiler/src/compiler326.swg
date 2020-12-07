#module "compiler326"
;

//#[swag.printbc]
#unittest error;
#test
{
	var a: [4] s32;
    #[swag.safety(true)]
	var c = a[5];
}
