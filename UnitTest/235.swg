#unittest module module235;
#unittest pass semantic;
using swag;

#[printByteCode]
func main: ()
{
	var a = 0xFFFFFF45;
	a &= 0xFF;
	@assert(a == 0x45);
	a |= 0x100;
	@assert(a == 0x145);
}

#run main;