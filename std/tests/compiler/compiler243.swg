;
;
using swag;

//#[printbc]
func main ()
{
	var a = 0;
	while a < 10
	{
		a += 1;
		while true
		{
			break;
		}		
	}
	
	@assert(a == 10);
}

#run 
{
	main()
}
