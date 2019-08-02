#module module450
#unittest pass semantic

//#[swag.printbytecode]
#unittest error
#test
{
	var a: *s32 = null;
	if(a[0] == 0)
	{
	}	
}