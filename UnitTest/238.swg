#unittest module module238;
#unittest pass semantic;
using swag;

//#[PrintByteCode]
func main: ()
{
	var a = 1;
	var b = 666;
	a = b;
	@assert(a == 666);
	b = 10;
	a += b;
	@assert(a == 676);
}

#run main;