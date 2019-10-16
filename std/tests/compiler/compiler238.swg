;
;
using swag;

//#[printbc]
func main ()
{
	var a = 1;
	var b = 666;
	a = b;
	@assert(a == 666);
	b = 10;
	a += b;
	@assert(a == 676);
}

#run 
{
	main()
}
