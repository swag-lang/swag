#module module546
//#unittest pass semantic

struct point
{
	var x = "10"
}

#[swag.printbytecode]
#test
{
	var pt: point
}