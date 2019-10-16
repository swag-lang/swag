#module compiler452
#unittest pass semantic

//#[swag.printbc]
#unittest error;
#test
{
	var a: string = null;
	if(a[0] == 1)
	{
	}
}