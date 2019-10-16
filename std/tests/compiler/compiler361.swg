#module compiler361
//#unittest pass semantic;

#unittest error;
#test
{
	const a = 1;
	var ptr = &a;
}
