#module module223;
#unittest pass semantic;
using swag;

var v = 50;

//#[printbytecode]
func main: ()
{
	@assert(v == 50);
	v = 666;
	@assert(v == 666);
}

#run main;
