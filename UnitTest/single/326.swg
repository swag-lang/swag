#module module326;
#unittest pass semantic;

//#[swag.printbc]
#unittest error;
#test
{
	var a: [4] s32;
	var c = a[5];
}
