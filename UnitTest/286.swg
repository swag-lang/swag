#module module286;
#unittest pass semantic;

//#[swag.printbytecode]
func main: ()
{
	var a: bool;
	var b = &a;
	var c = b[0];
}