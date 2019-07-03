#module module358;
//#unittest pass semantic;

var a = {1, 2};

#[swag.printbytecode]
#test
{
	var ptr = &a[0];
	//@print(ptr[0]);
}
