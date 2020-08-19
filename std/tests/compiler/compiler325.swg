#module "compiler325"
;

//#[swag.printbc]
#unittest error;
#test
{
	var a: [4] s32;
    #[swag.safety(true)]
	a[5] = 1;
}
