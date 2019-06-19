#module module229;
#unittest pass semantic;
using swag;

//#[printbytecode]
func main: ()
{
	var a : s32 = 666;
	@assert(a == 666);
	var b : s64 = 555;
	@assert(b == 555);
}

#run main;
