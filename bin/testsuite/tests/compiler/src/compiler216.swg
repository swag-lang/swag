;
;
using Swag;

//#[PrintBc]
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
