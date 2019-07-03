#module module361;
//#unittest pass semantic;

#[swag.printbytecode]
#test
{
	const a = 1;
	var ptr = &a;
}
