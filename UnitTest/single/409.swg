#module module409

#[swag.printbytecode]
#unittest error;
#test
{
	var c = "toto";
	var d: [..] u16 = c;
}