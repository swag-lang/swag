#module compiler418
#unittest pass semantic

//#[swag.printbc]
#unittest error;
#test
{
	const d = 6;
	d = 5;
}