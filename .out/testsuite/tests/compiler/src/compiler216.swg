;
;
using Swag;

//#[printbc]
func main ()
{
	if(false)
	{
		@assert(false);
	}
	else
	{
		@assert(true);
	}

	@assert(true);
}

#run
{
	main();
}
