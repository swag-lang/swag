#module module353;
//#unittest pass semantic;

#[swag.printbytecode]
#test
{
	var a = {"0", "1.0"};
	@print(a[0]);
}
