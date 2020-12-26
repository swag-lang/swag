;
;
using swag;

//#[printbc]
func main ()
{
	var a = 10;
	var b = 0;
	while(a > 0)
	{
		a = a - 1;
		b = b + 1;
	}
	
	@assert(a == 0);
	@assert(b == 10);
}

#run 
{
	main()
}

