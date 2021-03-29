;
;
using swag;

//#[printbc]
func main ()
{
	var a = 0b10101010;
	a >>= 1;
	@assert(a == 0b1010101);
	a <<= 2;
	@assert(a == 0b101010100);
}

#run
{
	main()
}
