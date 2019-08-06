#module module337;
#unittest pass semantic;

#unittest error;
//#[swag.printbc]
#test
{
	var a: [4, 6] s32;
	a[4, 0] = 1;
	//@print(a[0, 0]);
}
