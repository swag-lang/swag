#module module216;
#unittest pass semantic;
using swag;

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
