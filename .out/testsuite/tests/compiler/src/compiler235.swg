;
;
using Swag;

//#[PrintBc]
func main ()
{
	var a = 0xFFFFFF45;
	a &= 0xFF;
	@assert(a == 0x45);
	a |= 0x100;
	@assert(a == 0x145);

	var b = 0b1001;
	b ^= 0b1111;
	@assert(b == 0b0110);
}

#run
{
	main()
}
