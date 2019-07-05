#module module360;
//#unittest pass semantic;

var a = {1, 2};

//#[swag.printbytecode]
#unittest error;
#test
{
	var ptr = a;
}
