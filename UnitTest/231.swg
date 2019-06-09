#unittest module module231;
#unittest pass semantic;
using swag;

//#[printByteCode]
func main: ()
{
	var a = 10;
	a += 1;
	@assert(a == 11);
	
	var b = 1.0;
	b += 1.5;
	@assert(b == 2.5);
}

#run main;
